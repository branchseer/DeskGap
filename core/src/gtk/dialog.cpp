#include "./BrowserWindow_impl.h"
#include "../dialog/dialog.h"


namespace DeskGap {

    namespace {
        inline const char* NullableCStr(const std::optional<std::string>& str, const char* fallback = nullptr) {
            return str.has_value() ? str->c_str() : fallback;
        }
    }

    struct Dialog::Impl {

        static GtkFileChooserDialog* FileChooserDialogNew(
            std::optional<std::reference_wrapper<BrowserWindow>> browserWindow,
            GtkFileChooserAction action,
            const char* defaultCancelLabel,
            const char* defaultAcceptLabel,
            const Dialog::CommonFileDialogOptions& commonOptions
        ) {

            GtkWidget* dialog = gtk_file_chooser_dialog_new(
                NullableCStr(commonOptions.title),
                browserWindow.has_value() ? browserWindow->get().impl_->gtkWindow : nullptr,
                action,
                defaultCancelLabel,
                GTK_RESPONSE_CANCEL,
                NullableCStr(commonOptions.buttonLabel, defaultAcceptLabel),
                GTK_RESPONSE_ACCEPT,
                nullptr
            );

            if (commonOptions.defaultDirectory.has_value()) {
                gtk_file_chooser_set_current_folder(
                    GTK_FILE_CHOOSER(dialog),
                    commonOptions.defaultDirectory->c_str()
                );
            }
            if (commonOptions.defaultFilename.has_value()) {
                gtk_file_chooser_set_current_name(
                    GTK_FILE_CHOOSER(dialog),
                    commonOptions.defaultFilename->c_str()
                );
            }

            for (const auto& filter: commonOptions.filters) {
                GtkFileFilter* gtkFilter = gtk_file_filter_new();
                gtk_file_filter_set_name(gtkFilter, filter.name.c_str());
                for (const std::string& extension: filter.extensions) {
                    gtk_file_filter_add_pattern(gtkFilter, ("*." + extension).c_str());
                }
                gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), gtkFilter);
            }

            return GTK_FILE_CHOOSER_DIALOG(dialog);
        }

    };

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
        GtkFileChooserDialog* dialog = Impl::FileChooserDialogNew(
            browserWindow, 
            (options.properties & OpenDialogOptions::PROPERTY_OPEN_DIRECTORY) != 0 ? 
                GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER :
                GTK_FILE_CHOOSER_ACTION_OPEN,
            "Cancel", "Open",
            options.commonOptions
        );

        gtk_file_chooser_set_select_multiple(
            GTK_FILE_CHOOSER(dialog),
            (options.properties & OpenDialogOptions::PROPERTY_MULTI_SELECTIONS) != 0
        );

        gtk_file_chooser_set_show_hidden(
            GTK_FILE_CHOOSER(dialog),
            (options.properties & OpenDialogOptions::PROPERTY_SHOW_HIDDEN_FILES) != 0
        );

        Dialog::OpenDialogResult result;
        if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
            std::vector<std::string> filePaths;
            GSList *filenameList = gtk_file_chooser_get_filenames(GTK_FILE_CHOOSER (dialog));

            GSList* currentNode = filenameList;
            while (currentNode != nullptr) {
                filePaths.emplace_back(static_cast<const char*>(currentNode->data));
                currentNode = currentNode->next;
            }

            g_slist_free_full(filenameList, g_free);
            result.filePaths.emplace(std::move(filePaths));
        }
        gtk_widget_destroy(GTK_WIDGET(dialog));
        callback(std::move(result));
        // gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog), TRUE);

        // Dialog::SaveDialogResult result;
        // if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        //     char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER (dialog));
        //     result.filePath.emplace(filename);
        //     g_free(filename);
        // }
        // gtk_widget_destroy(GTK_WIDGET(dialog));
        // callback(std::move(result));
    }

    void Dialog::ShowSaveDialog(
        std::optional<std::reference_wrapper<BrowserWindow>> browserWindow,
        const SaveDialogOptions& options,
        Callback<SaveDialogResult>&& callback
    ) {
        GtkFileChooserDialog* dialog = Impl::FileChooserDialogNew(
            browserWindow, 
            GTK_FILE_CHOOSER_ACTION_SAVE,
            "Cancel", "Save",
            options.commonOptions
        );

        gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog), TRUE);

        Dialog::SaveDialogResult result;
        if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
            char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER (dialog));
            result.filePath.emplace(filename);
            g_free(filename);
        }
        gtk_widget_destroy(GTK_WIDGET(dialog));
        callback(std::move(result));
    }
}
