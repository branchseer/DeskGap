#include "shell.hpp"
#include "./util/wstring_utf8.h"
#include <Windows.h>
#include <shellapi.h>
#include <shlobj.h>
#include <atlbase.h>
#include <comdef.h>
#include <filesystem>
#include <wrl/client.h>

namespace fs = std::filesystem;

bool DeskGap::Shell::OpenExternal(const std::string &urlString)
{
    std::wstring wUrlString = UTF8ToWString(urlString.c_str());
    return ShellExecuteW(
               nullptr, L"open",
               wUrlString.c_str(),
               nullptr, nullptr,
               SW_SHOWNORMAL) > (HINSTANCE)32;
}

bool DeskGap::Shell::ShowItemInFolder(const std::string &path)
{
    // copy from electron ShowItemInFolder
    Microsoft::WRL::ComPtr<IShellFolder> desktop;
    HRESULT hr = SHGetDesktopFolder(desktop.GetAddressOf());
    if (FAILED(hr))
        return false;

    fs::path full_path(path);
    fs::path dir(full_path.parent_path());

    ITEMIDLIST *dir_item;
    hr = desktop->ParseDisplayName(NULL, NULL,
                                   const_cast<wchar_t *>(dir.c_str()),
                                   NULL, &dir_item, NULL);

    if (FAILED(hr))
        return false;

    ITEMIDLIST *file_item;
    hr = desktop->ParseDisplayName(
        NULL, NULL, const_cast<wchar_t *>(full_path.c_str()), NULL,
        &file_item, NULL);

    if (FAILED(hr))
        return false;

    const ITEMIDLIST *highlight[] = {file_item};
    hr = SHOpenFolderAndSelectItems(dir_item, std::size(highlight), highlight, NULL);

    if (FAILED(hr))
    {
        if (hr == ERROR_FILE_NOT_FOUND)
        {
            // On some systems, the above call mysteriously fails with "file not
            // found" even though the file is there.  In these cases, ShellExecute()
            // seems to work as a fallback (although it won't select the file).
            ShellExecute(NULL, L"open", dir.c_str(), NULL, NULL, SW_SHOW);
        }
    }

    CoTaskMemFree(dir_item);
    CoTaskMemFree(file_item);

    return true;
}
