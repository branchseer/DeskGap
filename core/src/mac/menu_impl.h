#ifndef mac_menu_impl_h
#define mac_menu_impl_h

#import <Cocoa/Cocoa.h>
#include <string>
#include "../menu/menu.h"

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
