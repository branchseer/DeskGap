#ifndef DESKGAP_MAC_MENU_IMPL_H
#define DESKGAP_MAC_MENU_IMPL_H

#import <Cocoa/Cocoa.h>
#include <string>
#include "menu.hpp"

namespace DeskGap {
    struct MenuItem::Impl {
        NSMenuItem* ns_menu_item;
        id item_target;
        std::string role;
    };
    
    struct Menu::Impl {
        NSMenu* ns_menu;
    };
}

#endif
