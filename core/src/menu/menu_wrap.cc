#include "menu_wrap.h"
#include "menu.h"
#include "../dispatch/dispatch.h"
#include <memory>
#include <vector>

namespace DeskGap {
    //MenuItemWrap Implementations Begin
    Napi::Function MenuItemWrap::Constructor(const Napi::Env& env) {
        return DefineClass(env, "MenuItemNative", {
            InstanceMethod("setLabel", &MenuItemWrap::SetLabel),
            InstanceMethod("getLabel", &MenuItemWrap::GetLabel),
            InstanceMethod("setEnabled", &MenuItemWrap::SetEnabled),
            InstanceMethod("setChecked", &MenuItemWrap::SetChecked),
            InstanceMethod("setAccelerator", &MenuItemWrap::SetAccelerator),
        });
    }

    MenuItemWrap::MenuItemWrap(const Napi::CallbackInfo& info):
            Napi::ObjectWrap<MenuItemWrap>(info)
    {
        std::string role = info[0].As<Napi::String>().Utf8Value();
        MenuItem::Type type = static_cast<MenuItem::Type>(info[1].As<Napi::Number>().Int32Value());

        MenuWrap* wrappedSubmenu = nullptr;
        if (!info[2].IsNull()) {
            wrappedSubmenu = MenuWrap::Unwrap(info[2].ToObject());
        }

        Napi::Object jsCallbacks = info[3].ToObject();
        Value().Set("callbacks_", jsCallbacks);

        MenuItem::EventCallbacks eventCallbacks {
            [jsOnClick = JSFunctionForUI::Weak(jsCallbacks.Get("onClick").As<Napi::Function>())]() {
                jsOnClick->Call();
            }
        };
        UISyncDelayable(info.Env(), [this, role, type, wrappedSubmenu, eventCallbacks = std::move(eventCallbacks)]() mutable {
            Menu* submenu = nullptr;
            if (wrappedSubmenu != nullptr) {
                submenu = wrappedSubmenu->menu_.get();
            }
            this->menu_item_ = std::make_unique<MenuItem>(role, type, submenu, std::move(eventCallbacks));
        });
    }

    Napi::Value MenuItemWrap::GetLabel(const Napi::CallbackInfo& info) {
        std::string label;
        UISync(info.Env(), [&]() {
            label = menu_item_->GetLabel();
        });
        return Napi::String::New(info.Env(), label);
    }

    void MenuItemWrap::SetLabel(const Napi::CallbackInfo& info) {
        std::string label = info[0].As<Napi::String>();
        UISyncDelayable(info.Env(), [this, label]() {
            this->menu_item_->SetLabel(label);
        });
    }
    void MenuItemWrap::SetEnabled(const Napi::CallbackInfo& info) {
        bool enabled = info[0].As<Napi::Boolean>().Value();
        UISyncDelayable(info.Env(), [this, enabled]() {
            this->menu_item_->SetEnabled(enabled);
        });
    }
    void MenuItemWrap::SetChecked(const Napi::CallbackInfo& info) {
        bool checked = info[0].As<Napi::Boolean>().Value();
        UISyncDelayable(info.Env(), [this, checked]() {
            this->menu_item_->SetChecked(checked);
        });
    }
    void MenuItemWrap::SetAccelerator(const Napi::CallbackInfo& info) {
        std::vector<std::string> tokens;
        Napi::Array tokenJSArray = info[0].As<Napi::Array>();
        uint32_t jsArrayLength = tokenJSArray.Length();
        for (uint32_t i = 0; i < jsArrayLength; ++i) {
            tokens.push_back(tokenJSArray.Get(i).As<Napi::String>().Utf8Value());
        }
        UISyncDelayable(info.Env(), [this, tokens]() {
            this->menu_item_->SetAccelerator(tokens);
        });
    }
    //MenuItemWrap Implementations End


    //MenuWrap Implementations Begin

    Napi::Function MenuWrap::Constructor(const Napi::Env& env) {
        return DefineClass(env, "MenuNative", {
            InstanceMethod("append", &MenuWrap::Append)
        });
    }

    MenuWrap::MenuWrap(const Napi::CallbackInfo& info):
        Napi::ObjectWrap<MenuWrap>(info)
    {
        Menu::Type menuType = static_cast<Menu::Type>(info[0].As<Napi::Number>().Int32Value());
        UISyncDelayable(info.Env(), [this, menuType]() {
            this->menu_ = std::make_unique<Menu>(menuType);
        });
    }
    void MenuWrap::Append(const Napi::CallbackInfo& info) {
        Napi::Object jsMenuItem = info[0].ToObject();
        MenuItemWrap* wrappedMenuItem = MenuItemWrap::Unwrap(jsMenuItem);
        
        UISyncDelayable(info.Env(), [this, wrappedMenuItem] {
            this->menu_->AppendItem(*(wrappedMenuItem->menu_item_)); 
        });
    }
    //MenuWrap Implementations End

}
