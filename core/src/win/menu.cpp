#include <functional>
#include <memory>
#include <unordered_map>

#include "../menu/menu.h"
#include "menu_impl.h"
#include "./util/string_convert.h"

using WinFormsMenuItem = System::Windows::Forms::MenuItem;
using WinFormsMenu = System::Windows::Forms::Menu;
using WinFormsMainMenu = System::Windows::Forms::MainMenu;
using WinFormsContextMenu = System::Windows::Forms::ContextMenu;
using namespace System;
using System::Windows::Forms::Shortcut;
using System::Windows::Forms::SendKeys;
using System::Windows::Forms::Application;

namespace DeskGap {
    namespace {
        ref class MenuItemCallbackHandler {
        private:
            MenuItem::EventCallbacks* const callbacks_;
        public:
            ~MenuItemCallbackHandler() { delete callbacks_; }
            MenuItemCallbackHandler(MenuItem::EventCallbacks* callbacks): callbacks_(new MenuItem::EventCallbacks(std::move(*callbacks))) { }

            void OnClick(Object^, EventArgs^) {
                callbacks_->onClick();
            }

            void SetOnClick(const std::function<void()>& newOnClick) {
                callbacks_->onClick = newOnClick;
            }
        };
    }

    MenuItem::MenuItem(const std::string& role, const Type& type, const Menu* submenu, EventCallbacks&& eventCallbacks): impl_(std::make_unique<Impl>()) {
        impl_->role = role;
        struct RoleInfo {
            std::string keys; Shortcut shortcut;
        };
        static const std::unordered_map<std::string, RoleInfo> kKeysAndShortcutsByRole {
            // This does not work every time. Need to find a more consistent way to perform these actions.
            // { "undo",   { "^z", Shortcut::CtrlZ } },
            // { "redo",   { "^y", Shortcut::CtrlY } },
            // { "cut",    { "^x", Shortcut::CtrlX } },
            // { "copy",   { "^c", Shortcut::CtrlC } },
            // { "paste",  { "^v", Shortcut::CtrlV } },
            // { "delete", { "{DEL}", Shortcut::Del } },
            // { "selectall",   { "^a", Shortcut::CtrlA } },
        };

        impl_->type = type;
        if (type == Type::SEPARATOR) {
            impl_->winforms_menu_item = gcnew WinFormsMenuItem("-");
        }
        else {
            WinFormsMenuItem^ item = nullptr;
            if (submenu != nullptr) {
                WinFormsMenu^ winforms_submenu = submenu->impl_->winforms_menu;
                item = safe_cast<WinFormsMenuItem^>(winforms_submenu);
            }
            else {
                item = gcnew WinFormsMenuItem();
                MenuItemCallbackHandler^ handler = gcnew MenuItemCallbackHandler(&eventCallbacks);                

                if (auto roleKeysIterator = kKeysAndShortcutsByRole.find(role); roleKeysIterator != kKeysAndShortcutsByRole.end()) {
                    item->Shortcut = roleKeysIterator->second.shortcut;
                    handler->SetOnClick([keys = roleKeysIterator->second.keys]() {
                        SendKeys::Send(ClrStr(keys));
                    });
                }
                else if (role == "toggledevtools") {
                    item->Text = "Install/Open Developer Tools...";
                    handler->SetOnClick([]() {
                        System::Diagnostics::Process::Start("microsoft-f12://");
                    });
                }

                item->Click += gcnew EventHandler(handler, &MenuItemCallbackHandler::OnClick);
            }
            impl_->winforms_menu_item = item;
        }
    }

    void MenuItem::SetLabel(const std::string& utf8label) {
        if (impl_->type == Type::SEPARATOR || impl_->role == "toggledevtools") return;
        impl_->winforms_menu_item->Text = ClrStr(utf8label);
        
    }
    
    void MenuItem::SetEnabled(bool enabled) {
        impl_->winforms_menu_item->Enabled = enabled;
    }


    void MenuItem::SetChecked(bool checked) {
        if (impl_->role == "toggledevtools") return;
        impl_->winforms_menu_item->Checked = checked;
    }
    
    std::string MenuItem::GetLabel() {
        return StdStr(impl_->winforms_menu_item->Text);
    }

    void MenuItem::SetAccelerator(const std::vector<std::string>& tokens) {
        //Not implemented yet.
        //The MenuItem class has the Shortcut property but it requires to be focused to be activated.
        //Maybe we need to register the accelerator inside the web page via JavaScript?
    }


    MenuItem::~MenuItem() = default;

    Menu::Menu(const Type& type): impl_(std::make_unique<Impl>()) {
        WinFormsMenu^ menu = nullptr;
        switch (type) {
        case Type::MAIN:
            menu = gcnew WinFormsMainMenu();
            break;
        case Type::CONTEXT:
            menu = gcnew WinFormsContextMenu();
            break;
        case Type::SUBMENU:
            menu = gcnew WinFormsMenuItem();
            break;
        }

        impl_->winforms_menu = menu;
    }

    void Menu::AppendItem(const MenuItem& menuItem) {
        impl_->winforms_menu->MenuItems->Add(menuItem.impl_->winforms_menu_item);
    }

    Menu::~Menu() = default;
}
