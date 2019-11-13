#ifndef DESKGAP_MENU_HPP
#define DESKGAP_MENU_HPP

#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <optional>

namespace DeskGap {
    class Menu;
    class MenuItem;

    class MenuItem {
    private:
        friend class Menu;
        struct Impl;
        std::unique_ptr<Impl> impl_;
    public:
        enum class Type: int {
            NORMAL = 0, SEPARATOR = 1, SUBMENU = 2, CHECKBOX = 3, RADIO = 4
        };
        MenuItem(const MenuItem&) = delete;

        struct EventCallbacks {
            std::function<void()> onClick;
        };
        MenuItem(const std::string& role, const Type&, const Menu* submenu, EventCallbacks&&);
        std::string GetLabel();
        void SetLabel(const std::string& utf8label);
        void SetEnabled(bool enabled);
        void SetAccelerator(const std::vector<std::string>& tokens);
        void SetChecked(bool checked);
        ~MenuItem();
    };

    class Menu {
    private:
        friend class MenuItem;
        friend class BrowserWindow;
        friend class App;
        struct Impl;
        std::unique_ptr<Impl> impl_;
    public:
        enum class Type: int {
            MAIN = 0, CONTEXT = 1, SUBMENU = 2
        };
        Menu(const Menu&) = delete;
        Menu(const Type&);
        void AppendItem(const MenuItem& menuItem);
        
        ~Menu();
    };
}

#endif
