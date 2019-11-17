#include <Windows.h>
#include <Commctrl.h>
#include <Shobjidl.h> 
#include <atlbase.h>
#include <cassert> 
#include <sstream> 

#include "util/win32_check.h"
#include "./BrowserWindow_impl.h"
#include "dialog.hpp"

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

    struct Dialog::Impl { };

    namespace {
        void PrepareDialog(IFileDialog* dialog, const CommonFileDialogOptions& options) {
            if (options.defaultDirectory.has_value()) {
                std::wstring wfolder = UTF8ToWString(options.defaultDirectory->c_str());
                ATL::CComPtr<IShellItem> folderItem;
                HRESULT hresult = SHCreateItemFromParsingName(
                    wfolder.c_str(), nullptr, IID_PPV_ARGS(&folderItem)
                );
                if (SUCCEEDED(hresult)) {
                    dialog->SetFolder(folderItem);
                }
            }

            if (options.defaultFilename.has_value()) {
                std::wstring wFilename = UTF8ToWString(options.defaultFilename->c_str());
                dialog->SetFileName(wFilename.c_str());
            }

            if (options.title.has_value()) {
                std::wstring wTitle = UTF8ToWString(options.title->c_str());
                dialog->SetTitle(wTitle.c_str());
            }

            if (options.buttonLabel.has_value()) {
                std::wstring wButtonLabel = UTF8ToWString(options.buttonLabel->c_str());
                dialog->SetOkButtonLabel(wButtonLabel.c_str());
            }

            if (!options.filters.empty()) {
                std::vector<std::wstring> pszNameBuffer;
                std::vector<std::wstring> pszSpecBuffer;
                pszNameBuffer.reserve(options.filters.size());
                pszSpecBuffer.reserve(options.filters.size());

                std::vector<COMDLG_FILTERSPEC> filterspecs(options.filters.size());

                for (size_t i = 0; i < options.filters.size(); ++i) {
                    const auto& filter = options.filters[i];
                    COMDLG_FILTERSPEC& spec = filterspecs[i];

                    pszNameBuffer.push_back(UTF8ToWString(filter.name.c_str()));
                    spec.pszName = pszNameBuffer.back().c_str();

                    if (filter.extensions.empty()) {
                        pszSpecBuffer.emplace_back(L"*.*");
                    }
                    else {
                        std::wostringstream pszNameStream;
                        for (const std::string& extension: filter.extensions) {
                            pszNameStream << L"*.";
                            pszNameStream << UTF8ToWString(extension.c_str());
                            if (&extension != &(filter.extensions.back())) {
                                pszNameStream << L";";
                            }
                        }
                        pszSpecBuffer.push_back(pszNameStream.str());
                    }
                    spec.pszSpec = pszSpecBuffer.back().c_str();
                }

                dialog->SetFileTypes(filterspecs.size(), filterspecs.data());

                //Reason to do this: https://github.com/electron/electron/blob/6cb7b8d3a4b0ec2e6c1edb314dc32f126882cf37/atom/browser/ui/file_dialog_win.cc#L195
                for (size_t i = 0; i < filterspecs.size(); ++i) {
                    if (wcscmp(filterspecs[i].pszSpec, L"*.*") != 0) {
                      // SetFileTypeIndex is regarded as one-based index.
                      dialog->SetFileTypeIndex(i + 1);
                      dialog->SetDefaultExtension(filterspecs[i].pszSpec);
                      break;
                    }
                }
            }
        }

    }

    void Dialog::ShowOpenDialog(
        std::optional<std::reference_wrapper<BrowserWindow>> browserWindow,
        const OpenDialogOptions& options,
        Callback<OpenDialogResult>&& callback
    ) {

        ATL::CComPtr<IFileOpenDialog> openDialog;
        check(openDialog.CoCreateInstance(CLSID_FileOpenDialog));

        DWORD fosOptions = FOS_FORCEFILESYSTEM | FOS_FILEMUSTEXIST;
        if (options.properties & OpenDialogOptions::PROPERTY_OPEN_DIRECTORY)
            fosOptions |= FOS_PICKFOLDERS;
        if (options.properties & OpenDialogOptions::PROPERTY_MULTI_SELECTIONS)
            fosOptions |= FOS_ALLOWMULTISELECT;
        if (options.properties & OpenDialogOptions::PROPERTY_SHOW_HIDDEN_FILES)
            fosOptions |= FOS_FORCESHOWHIDDEN;
        if (options.properties & OpenDialogOptions::PROPERTY_PROMPT_TO_CREATE)
            fosOptions |= FOS_CREATEPROMPT;
        check(openDialog->SetOptions(fosOptions));

        PrepareDialog(openDialog, options.commonOptions);

        openDialog->Show(
            browserWindow.has_value() ? browserWindow->get().impl_->windowWnd : nullptr
        );

        Dialog::OpenDialogResult result;
        ATL::CComPtr<IShellItemArray> items;
        if (SUCCEEDED(openDialog->GetResults(&items))) {
            result.filePaths.emplace();

            DWORD itemCount;
            check(items->GetCount(&itemCount));

            result.filePaths->reserve(itemCount);
            for (DWORD i = 0; i < itemCount; ++i) {
                ATL::CComPtr<IShellItem> item;
                check(items->GetItemAt(i, &item));

                LPWSTR filePath;
                check(item->GetDisplayName(SIGDN_FILESYSPATH, &filePath));
                result.filePaths->push_back(WStringToUTF8(filePath));
                CoTaskMemFree(filePath);
            }
        }
        callback(std::move(result));
    }

    void Dialog::ShowSaveDialog(
        std::optional<std::reference_wrapper<BrowserWindow>> browserWindow,
        const SaveDialogOptions& options,
        Callback<SaveDialogResult>&& callback
    ) {
        
        ATL::CComPtr<IFileSaveDialog> saveDialog;
        check(saveDialog.CoCreateInstance(CLSID_FileSaveDialog));

        check(saveDialog->SetOptions(
            FOS_FORCEFILESYSTEM | FOS_PATHMUSTEXIST | FOS_OVERWRITEPROMPT
        ));

        saveDialog->Show(
            browserWindow.has_value() ? browserWindow->get().impl_->windowWnd : nullptr
        );

        Dialog::SaveDialogResult result;

        CComPtr<IShellItem> item;
        if (SUCCEEDED(saveDialog->GetResult(&item))) {
            LPWSTR filePath;
            check(item->GetDisplayName(SIGDN_FILESYSPATH, &filePath));
            result.filePath.emplace(WStringToUTF8(filePath));
            CoTaskMemFree(filePath);
        }
        callback(std::move(result));
    }
}
