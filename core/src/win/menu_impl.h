#ifndef win_menu_impl_h
#define win_menu_impl_h

#include <vcclr.h> 
#include "../menu/menu.h"

namespace DeskGap {
    struct MenuItem::Impl {
        gcroot<System::Windows::Forms::MenuItem^> winforms_menu_item;
        MenuItem::Type type;
        std::string role;
    };
    
    struct Menu::Impl {
        gcroot<System::Windows::Forms::Menu^> winforms_menu;
    };
}

#endif
