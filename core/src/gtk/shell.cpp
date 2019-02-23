#include "../shell/shell.h"

#include <gtk/gtk.h>

bool DeskGap::Shell::OpenExternal(const std::string& urlString) {
    return gtk_show_uri_on_window(nullptr, urlString.c_str(), GDK_CURRENT_TIME, nullptr);
}
