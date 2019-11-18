#ifndef menu_menu_wrap_h
#define menu_menu_wrap_h

#include <napi.h>
#include <functional>
#include <memory>
#include <deskgap/menu.hpp>
//#include "menu.h"

namespace DeskGap {
    class MenuItemWrap: public Napi::ObjectWrap<MenuItemWrap> {
    private:
        friend class MenuWrap;
        std::unique_ptr<MenuItem> menu_item_;

        void SetLabel(const Napi::CallbackInfo& info);
        Napi::Value GetLabel(const Napi::CallbackInfo& info);
        void SetEnabled(const Napi::CallbackInfo& info);
        void SetChecked(const Napi::CallbackInfo& info);
        void SetAccelerator(const Napi::CallbackInfo& info);
        void Destroy(const Napi::CallbackInfo& info);
    public:
        MenuItemWrap(const Napi::CallbackInfo& info);

        static Napi::Function Constructor(const Napi::Env& env);
    };

    class MenuWrap: public Napi::ObjectWrap<MenuWrap> {
    private:
        friend class MenuItemWrap;
        friend class BrowserWindowWrap;
        friend class AppWrap;
        std::unique_ptr<Menu> menu_;
        void Append(const Napi::CallbackInfo& info);
        void Destroy(const Napi::CallbackInfo& info);
    public:
        MenuWrap(const Napi::CallbackInfo& info);
        static Napi::Function Constructor(const Napi::Env& env);
    };
}

#endif
