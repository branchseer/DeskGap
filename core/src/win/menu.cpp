#include <functional>
#include <memory>
#include <unordered_map>

#include "../menu/menu.h"
#include "menu_impl.h"
#include "util/wstring_utf8.h"

namespace {
    UINT_PTR lastMenuItemId = 0;
    void SendKey(WORD key, const std::function<void()>& whileHolding = []() { }) {
        INPUT input { };
        input.type = INPUT_KEYBOARD;

        input.ki.wVk = key;
        input.ki.dwFlags = 0;
        SendInput(1, &input, sizeof(INPUT));

        whileHolding();

        input.ki.wVk = key;
        input.ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(1, &input, sizeof(INPUT));
    }
    void SendCtrl(char ch) {
        SendKey(VK_CONTROL, [ch]() {
            SendKey(ch);
        });
    }
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
        UINT flags = MF_ENABLED;
        if (type == Type::SUBMENU) {
            flags |= MF_POPUP;
        }
        else if (type == Type::SEPARATOR) {
            flags |= MF_SEPARATOR;
        }
        else {
            flags |= MF_STRING;
        }
        AppendMenuW(parentHMenu, flags, identifier, L"");

        while (!pendingActions_.empty()) {
            pendingActions_.front()(parentHMenu);
            pendingActions_.pop();
        }
    }
    MenuItem::MenuItem(const std::string& role, const Type& type, const Menu* submenu, EventCallbacks&& eventCallbacks): impl_(std::make_unique<Impl>()) {
        impl_->callbacks = std::move(eventCallbacks);
        impl_->role = role;
        impl_->type = type;
        impl_->enabled = true;
        impl_->checked = false;
        if (submenu != nullptr) {
            impl_->identifier = UINT_PTR(submenu->impl_->hmenu);
        }
        else {
            impl_->identifier = ++lastMenuItemId;
        }

        if (role == "toggledevtools") {
            impl_->SetLabel("Install/Open Developer Tools...");
            impl_->callbacks.onClick = []() {
                bool success = ShellExecuteW(
                    nullptr, L"open",
                    L"microsoft-f12://",
                    nullptr, nullptr,
                    SW_SHOWNORMAL
                ) > (HINSTANCE)32;

                if (!success) {
                    ShellExecuteW(
                        nullptr, L"open",
                        L"ms-windows-store://pdp/?ProductId=9MZBFRMZ0MNJ",
                        nullptr, nullptr,
                        SW_SHOWNORMAL
                    );
                }
            };
        }
        else if (role == "copy") {
            impl_->callbacks.onClick = []() { SendCtrl('C'); };
        }
        else if (role == "cut") {
            impl_->callbacks.onClick = []() { SendCtrl('X'); };
        }
        else if (role == "paste") {
            impl_->callbacks.onClick = []() { SendCtrl('V'); };
        }
        else if (role == "selectall") {
            impl_->callbacks.onClick = []() { SendCtrl('A'); };
        }
        else if (role == "undo") {
            impl_->callbacks.onClick = []() { SendCtrl('Z'); };
        }
        else if (role == "redo") {
            impl_->callbacks.onClick = []() { SendCtrl('Y'); };
        }
        else if (role == "delete") {
            impl_->callbacks.onClick = []() { SendKey(VK_BACK); };
        }
        impl_->UpdateState();
    }

    void MenuItem::SetLabel(const std::string& utf8label) {
        if (this->impl_->type == Type::SEPARATOR || this->impl_->role == "toggledevtools") return;
        this->impl_->SetLabel(utf8label);
    }

    void MenuItem::Impl::SetLabel(const std::string& utf8label) {
        AddAction([
            this,
            wlabel = UTF8ToWString(utf8label.c_str())
        ](HMENU parentHMenu) {
            MENUITEMINFOW info { };
            info.cbSize = sizeof(info);
            info.fMask = MIIM_STRING | MIIM_DATA;
            info.dwTypeData = LPWSTR(wlabel.c_str());
            SetMenuItemInfoW(parentHMenu, this->identifier, FALSE, &info);
        });
    }

    
    void MenuItem::SetEnabled(bool enabled) {
        this->impl_->enabled = enabled;
        this->impl_->UpdateState();
    }


    void MenuItem::SetChecked(bool checked) {
        this->impl_->checked = checked;
        this->impl_->UpdateState();
    }


    void MenuItem::Impl::UpdateState() {
        AddAction([this](HMENU parentHMenu) {
            MENUITEMINFOW info { };
            info.cbSize = sizeof(info);
            info.fMask = MIIM_STATE;
            info.fState =
                (this->checked ? MFS_CHECKED : MFS_UNCHECKED) |
                (this->enabled ? MFS_ENABLED : MFS_DISABLED);
            SetMenuItemInfoW(parentHMenu, this->identifier, FALSE, &info);
        });
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

        //Make menu items send WM_MENUCOMMAND
        MENUINFO info { };
        info.cbSize = sizeof(info);
        info.fMask = MIM_STYLE | MIM_MENUDATA;
        info.dwStyle = MNS_NOTIFYBYPOS;
        info.dwMenuData = reinterpret_cast<ULONG_PTR>(&(impl_->clickHandlers));
        SetMenuInfo(impl_->hmenu, &info);
    }

    void Menu::AppendItem(const MenuItem& menuItem) {
        impl_->clickHandlers.push_back(&(menuItem.impl_->callbacks.onClick));
        menuItem.impl_->AppendTo(impl_->hmenu);
    }

    Menu::~Menu() = default;
}
