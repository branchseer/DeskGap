#ifndef gtk_menu_impl_h
#define gtk_menu_impl_h

#include <gtk/gtk.h>
#include "../menu/menu.h"

namespace DeskGap {
    struct MenuItem::Impl {
    	GtkMenuItem* gtkMenuItem;
    };
    
    struct Menu::Impl {

    };
}

#endif
