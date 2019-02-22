#ifndef gtk_browserwindow_impl_h
#define gtk_browserwindow_impl_h

#include <gtk/gtk.h>
#include <memory>

#include "../window/browser_window.h"

namespace DeskGap {
    struct BrowserWindow::Impl {
    	GtkWindow* gtkWindow;
    	GtkBox* gtkBox;

    	struct AccelGroupMenu
    	{
    		GtkAccelGroup* accelGroup;
    		GtkWidget* menuBar;
    		AccelGroupMenu(const Menu& menu);
    		~AccelGroupMenu();
    	};
    	std::optional<AccelGroupMenu> accelGroupMenu;

    	BrowserWindow::EventCallbacks callbacks;
    	gulong deleteEventConnection;
    	static bool HandleDeleteEvent(GtkWidget*, GdkEvent*, BrowserWindow*);
    };
}

#endif
