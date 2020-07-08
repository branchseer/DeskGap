#ifndef win_menu_impl_h
#define win_menu_impl_h

#include <Windows.h>
#include <optional>
#include <queue>
#include <functional>
#include <vector>

#include "menu.hpp"

namespace DeskGap {
    struct MenuItem::Impl {
        //Top-level MenuItems in a Window menu needs to know the hwnd of the window for DrawMenuBar
        HWND windowWnd;

        std::string role;
        Type type;
        UINT_PTR identifier;

        std::wstring wlabel;
        bool enabled;
        bool checked;
        void UpdateInfo();

        MenuItem::EventCallbacks callbacks;

        std::optional<HMENU> parentHMenu_;

        void AppendTo(HMENU parentHMenu);
    };
    
    struct Menu::Impl {
        Type type;
        
        std::vector<std::function<void()>*> clickHandlers;
        std::vector<const MenuItem*> items;
        void SetWindowWnd(HWND windowWnd);
        HMENU hmenu;
    };
}

#endif
