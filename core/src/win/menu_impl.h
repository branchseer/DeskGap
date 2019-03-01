#ifndef win_menu_impl_h
#define win_menu_impl_h

#include <Windows.h>
#include <optional>
#include <queue>
#include <functional>

#include "../menu/menu.h"

namespace DeskGap {
    struct MenuItem::Impl {
    	std::string role;
    	Type type;
    	UINT_PTR identifier;

    	using Action = std::function<void(HMENU)>;
    	//pending changes before CreateMenu 
        std::queue<Action> pendingActions_;
        std::optional<HMENU> parentHMenu_;

        void AddAction(Action&& action);
        void AppendTo(HMENU parentHMenu);
    };
    
    struct Menu::Impl {
    	HMENU hmenu;
    };
}

#endif
