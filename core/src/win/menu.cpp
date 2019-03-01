#include <functional>
#include <memory>
#include <unordered_map>

#include "../menu/menu.h"
#include "menu_impl.h"
#include "util/wstring_utf8.h"

namespace {
    UINT_PTR lastMenuItemId = 0;
}

namespace DeskGap {
    void MenuItem::Impl::AddAction(Action&& action) {
        if (parentHMenu_.has_value()) {
            action(*parentHMenu_);
        }
        else {
            pendingActions_.emplace(std::move(action));
        }
    }
    void MenuItem::Impl::AppendTo(HMENU parentHMenu) {
        parentHMenu_.emplace(parentHMenu);
        UINT flags = MF_ENABLED | MF_STRING;
        if (type == Type::SUBMENU) {
            flags |= MF_POPUP;
        }
        AppendMenuW(parentHMenu, flags, identifier, L"");

        while (!pendingActions_.empty()) {
            pendingActions_.front()(parentHMenu);
            pendingActions_.pop();
        }
    }
    MenuItem::MenuItem(const std::string& role, const Type& type, const Menu* submenu, EventCallbacks&& eventCallbacks): impl_(std::make_unique<Impl>()) {
        impl_->role = role;
        impl_->type = type;
        if (submenu != nullptr) {
            impl_->identifier = UINT_PTR(submenu->impl_->hmenu);
        }
        else {
            impl_->identifier = ++lastMenuItemId;
        }
    }

    void MenuItem::SetLabel(const std::string& utf8label) {
        impl_->AddAction([
            this,
            wlabel = UTF8ToWString(utf8label.c_str())
        ](HMENU parentHMenu) {
            MENUITEMINFOW info { };
            info.cbSize = sizeof(info);
            info.fMask = MIIM_STRING | MIIM_DATA;
            info.dwTypeData = LPWSTR(wlabel.c_str());
            SetMenuItemInfoW(parentHMenu, this->impl_->identifier, FALSE, &info);
        });
    }
    
    void MenuItem::SetEnabled(bool enabled) {
        
    }


    void MenuItem::SetChecked(bool checked) {
        
    }
    
    std::string MenuItem::GetLabel() {
        return { };
    }

    void MenuItem::SetAccelerator(const std::vector<std::string>& tokens) {
        //Not implemented yet.
        //The MenuItem class has the Shortcut property but it requires to be focused to be activated.
        //Maybe we need to register the accelerator inside the web page via JavaScript?
    }


    MenuItem::~MenuItem() = default;

    Menu::Menu(const Type& type): impl_(std::make_unique<Impl>()) {
        if (type == Type::MAIN) {
            impl_->hmenu = CreateMenu();
        }
        else {
            impl_->hmenu = CreatePopupMenu();
        }
    }

    void Menu::AppendItem(const MenuItem& menuItem) {
        menuItem.impl_->AppendTo(impl_->hmenu);
    }

    Menu::~Menu() = default;
}
