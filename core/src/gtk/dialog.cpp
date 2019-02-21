#include "./BrowserWindow_impl.h"
#include "../dialog/dialog.h"

namespace DeskGap {
    void Dialog::ShowErrorBox(const std::string& title, const std::string& content) {
        GtkWidget* dialog = gtk_message_dialog_new(
            nullptr,
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_CLOSE,
            title.c_str(),
            content.c_str()
        );
        gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog), "%s", content.c_str());
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    }

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
