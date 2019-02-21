#include "../menu/menu.h"
#include "menu_impl.h"

namespace DeskGap {

    MenuItem::MenuItem(const std::string& role, const Type& type, const Menu* submenu, EventCallbacks&& eventCallbacks): impl_(std::make_unique<Impl>()) {
        //impl_->Gtk
    }

    void MenuItem::SetLabel(const std::string& utf8label) {

    }
    
    void MenuItem::SetEnabled(bool enabled) {

    }


    void MenuItem::SetChecked(bool checked) {

    }
    
    std::string MenuItem::GetLabel() {
        return "";
    }

    void MenuItem::SetAccelerator(const std::vector<std::string>& tokens) {

    }


    MenuItem::~MenuItem() = default;

    Menu::Menu(const Type& type): impl_(std::make_unique<Impl>()) {

    }

    void Menu::AppendItem(const MenuItem& menuItem) {

    }

    Menu::~Menu() = default;
}
