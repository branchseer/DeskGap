#ifndef gtk_browserwindow_impl_h
#define gtk_browserwindow_impl_h

#include <gtk/gtk.h>
#include <memory>

#include "../window/browser_window.h"

namespace DeskGap {
    struct BrowserWindow::Impl {
    	GtkWindow* gtkWindow;
    	GtkBox* gtkBox;
    	GtkWidget* menuBar;

    	BrowserWindow::EventCallbacks callbacks;
    	gulong deleteEventConnection;
    	static bool HandleDeleteEvent(GtkWidget*, GdkEvent*, BrowserWindow*);
    };
}

#endif
