#ifndef DESKGAP_DIALOG_HPP
#define DESKGAP_DIALOG_HPP

#include <string>
#include <functional>
#include <optional>
#include <vector>
#include "browser_window.hpp"

namespace DeskGap {
    class Dialog {
    public:
        static void ShowErrorBox(const std::string& title, const std::string& content);
        struct CommonFileDialogOptions {
            std::optional<std::string> title;
            std::optional<std::string> defaultDirectory;
            std::optional<std::string> defaultFilename;
            std::optional<std::string> buttonLabel;
            struct FileFilter {
                std::string name;
                std::vector<std::string> extensions;
            };
            std::vector<FileFilter> filters;
            std::optional<std::string> message;
        };

        struct OpenDialogOptions {
            CommonFileDialogOptions commonOptions;

            using Properties = uint32_t;
            static const Properties PROPERTY_OPEN_FILE = 1 << 0;
            static const Properties PROPERTY_OPEN_DIRECTORY = 1 << 1;
            static const Properties PROPERTY_MULTI_SELECTIONS = 1 << 2;
            static const Properties PROPERTY_SHOW_HIDDEN_FILES = 1 << 3;
            static const Properties PROPERTY_CREATE_DIRECTORY = 1 << 4;
            static const Properties PROPERTY_PROMPT_TO_CREATE = 1 << 5;
            static const Properties PROPERTY_NO_RESOLVE_ALIASES = 1 << 6;
            static const Properties PROPERTY_TREAT_PACKAGE_AS_DIRECTORY = 1 << 7;

            Properties properties;
        };

        struct SaveDialogOptions {
            CommonFileDialogOptions commonOptions;
            std::optional<std::string> nameFieldLabel;
            std::optional<bool> showsTagField;
        };

        struct OpenDialogResult {
            std::optional<std::vector<std::string>> filePaths;
        };

        struct SaveDialogResult {
            std::optional<std::string> filePath;
        };

        struct Impl;

        template<class Result>
        using Callback = std::function<void(Result&&)>;

        static void ShowOpenDialog(
            std::optional<std::reference_wrapper<BrowserWindow>> browserWindow,
            const OpenDialogOptions& options,
            Callback<OpenDialogResult>&& callback
        );
        static void ShowSaveDialog(
            std::optional<std::reference_wrapper<BrowserWindow>> browserWindow,
            const SaveDialogOptions& options,
            Callback<SaveDialogResult>&& callback
        );
    };
}

#endif
