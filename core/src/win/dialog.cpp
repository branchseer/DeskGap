#include <Windows.h>
#include <Commctrl.h>

#include "./BrowserWindow_impl.h"
#include "../dialog/dialog.h"

#include "./util/wstring_utf8.h"

#pragma comment(lib, "Comctl32")

using CommonFileDialogOptions = DeskGap::Dialog::CommonFileDialogOptions;
using FileFilter = CommonFileDialogOptions::FileFilter;

namespace DeskGap {
    void Dialog::ShowErrorBox(const std::string& title, const std::string& content) {
        TASKDIALOGCONFIG config { };
        config.cbSize = sizeof(config);
        config.hInstance = GetModuleHandleW(NULL);
        config.dwFlags = TDF_SIZE_TO_CONTENT;
        config.pszWindowTitle = L"Error";
        config.pszMainIcon = TD_ERROR_ICON;

        std::wstring wtitle = UTF8ToWString(title.c_str());
        std::wstring wcontent = UTF8ToWString(content.c_str());
        config.pszMainInstruction = wtitle.c_str();
        config.pszContent =wcontent.c_str();
        TaskDialogIndirect(&config, nullptr, nullptr, nullptr);
    }

    struct Dialog::Impl {
        
    };

    void Dialog::ShowOpenDialog(
        std::optional<std::reference_wrapper<BrowserWindow>> browserWindow,
        const OpenDialogOptions& options,
        Callback<OpenDialogResult>&& callback
    ) {
       
    }

    void Dialog::ShowSaveDialog(
        std::optional<std::reference_wrapper<BrowserWindow>> browserWindow,
        const SaveDialogOptions& options,
        Callback<SaveDialogResult>&& callback
    ) {
        
    }
}
