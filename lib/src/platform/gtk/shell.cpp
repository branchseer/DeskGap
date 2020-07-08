#include "shell.hpp"

#include <gtk/gtk.h>

bool DeskGap::Shell::OpenExternal(const std::string& urlString) {
    return gtk_show_uri(nullptr, urlString.c_str(), GDK_CURRENT_TIME, nullptr);
}

bool DeskGap::Shell::ShowItemInFolder(const std::string& path) {
    return false;
}
