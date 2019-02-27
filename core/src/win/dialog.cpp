#include "./BrowserWindow_impl.h"
#include "../dialog/dialog.h"


using CommonFileDialogOptions = DeskGap::Dialog::CommonFileDialogOptions;
using FileFilter = CommonFileDialogOptions::FileFilter;

namespace DeskGap {
    void Dialog::ShowErrorBox(const std::string& title, const std::string& content) {
        
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
