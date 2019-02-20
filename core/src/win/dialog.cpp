#include "./BrowserWindow_impl.h"
#include "../dialog/dialog.h"
#include "./util/string_convert.h"

using namespace System;
using namespace System::Windows::Forms;


using CommonFileDialogOptions = DeskGap::Dialog::CommonFileDialogOptions;
using FileFilter = CommonFileDialogOptions::FileFilter;

namespace DeskGap {
    void Dialog::ShowErrorBox(const std::string& title, const std::string& content) {
        MessageBox::Show(ClrStr(title + "\n\n" + content), "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
    }

    struct Dialog::Impl {
        static DialogResult PresentDialog(std::optional<std::reference_wrapper<BrowserWindow>> browserWindow, CommonDialog^ dialog) {
            if (browserWindow.has_value()) {
                Form^ form = browserWindow.value()->impl_->form;
                return dialog->ShowDialog(form);
            }
            else {
                return dialog->ShowDialog();
            }
        }
    };

    namespace {
        void PrepareDialog(FileDialog^ fileDialog, const CommonFileDialogOptions& options) {
            if (options.title.has_value()) {
                fileDialog->Title = ClrStr(options.title.value());
            }

            if (options.filters.empty()) {
                fileDialog->Filter = "All files (*.*)|*.*";
            }
            else {
                array<String^>^ filterStrings = gcnew array<String^>(options.filters.size());
                for (size_t i = 0; i < options.filters.size(); ++i) {
                    const FileFilter& filter = options.filters[i];
                    if (filter.extensions.empty()) {
                        filterStrings[i] = ClrStr(filter.name + "|*.*");
                    }
                    else {
                        array<String^>^ extensions = gcnew array<String^>(filter.extensions.size());
                        for (size_t j = 0; j < filter.extensions.size(); ++j) {
                            extensions[j] = ClrStr("*." + filter.extensions[j]);
                        }
                        filterStrings[i] = ClrStr(filter.name + "|") + String::Join(";", extensions);
                    }
                }
                fileDialog->Filter = String::Join("|", filterStrings);
            }

            if (options.defaultDirectory.has_value()) {
                fileDialog->InitialDirectory = ClrStr(options.defaultDirectory.value());
            }
            if (options.defaultFilename.has_value()) {
                fileDialog->FileName = ClrStr(options.defaultFilename.value());
            }
        }
    }

    void Dialog::ShowOpenDialog(
        std::optional<std::reference_wrapper<BrowserWindow>> browserWindow,
        const OpenDialogOptions& options,
        Callback<OpenDialogResult>&& callback
    ) {
        Dialog::OpenDialogResult result;
        if (options.properties & OpenDialogOptions::PROPERTY_OPEN_DIRECTORY) {
            FolderBrowserDialog^ dialog = gcnew FolderBrowserDialog();
            if (const auto& defaultDirectory = options.commonOptions.defaultDirectory; defaultDirectory.has_value()) {
                dialog->SelectedPath = ClrStr(defaultDirectory.value());
                if (Impl::PresentDialog(browserWindow, dialog) == DialogResult::OK) {
                    result.filePaths.emplace({ StdStr(dialog->SelectedPath) });
                }
            }
        } 
        else {
            OpenFileDialog^ dialog = gcnew OpenFileDialog();
            PrepareDialog(dialog, options.commonOptions);
            dialog->Multiselect = (options.properties & OpenDialogOptions::PROPERTY_MULTI_SELECTIONS) != 0;

            if (Impl::PresentDialog(browserWindow, dialog) == DialogResult::OK) {
                std::vector<std::string> filePaths;
                array<String^>^ clrFilePaths = dialog->FileNames;
                filePaths.reserve(clrFilePaths->Length);
                for each(String^ filePath in clrFilePaths) {
                    filePaths.push_back(StdStr(filePath));
                }
                result.filePaths.emplace(std::move(filePaths));
            }
        }
        callback(std::move(result));
    }

    void Dialog::ShowSaveDialog(
        std::optional<std::reference_wrapper<BrowserWindow>> browserWindow,
        const SaveDialogOptions& options,
        Callback<SaveDialogResult>&& callback
    ) {
        SaveFileDialog^ dialog = gcnew SaveFileDialog();
        PrepareDialog(dialog, options.commonOptions);
        SaveDialogResult result;
        if (Impl::PresentDialog(browserWindow, dialog) == DialogResult::OK) {
            result.filePath.emplace(StdStr(dialog->FileName));
        }
        callback(std::move(result));
    }
}
