#ifndef gtk_browserwindow_impl_h
#define gtk_browserwindow_impl_h

#include <gtkmm.h>
#include <memory>

#include "../window/browser_window.h"

namespace DeskGap {
    struct BrowserWindow::Impl {
    	std::unique_ptr<Gtk::Window> gtkWindow;
    };
}

#endif
