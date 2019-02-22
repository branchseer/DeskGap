#ifndef gtk_menu_impl_h
#define gtk_menu_impl_h

#include <functional>
#include <optional>
#include <vector>
#include <gtk/gtk.h>
#include "../menu/menu.h"

namespace DeskGap {
    struct MenuItem::Impl {
        GtkMenuItem* gtkMenuItem;
        gulong activateConnection;

        MenuItem::EventCallbacks callbacks;
        static void HandleActivate(GtkMenuItem*, MenuItem*);

        struct AccelInfo
        {
        	guint key;
        	GdkModifierType mods;
        };
        std::optional<AccelInfo> accelInfo;
        std::optional<std::reference_wrapper<const Menu>> submenu;
    };
    
    struct Menu::Impl {
        GtkMenuShell* gtkMenuShell;
        std::vector<std::reference_wrapper<const MenuItem>> items;

        void SetAccelGroup(GtkAccelGroup* accelGroup) const;
    };
}

#endif
