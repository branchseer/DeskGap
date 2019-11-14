#include "menu_wrap.h"
#include <deskgap/menu.hpp>
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
            InstanceMethod("destroy", &MenuItemWrap::Destroy),
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


        MenuItem::EventCallbacks eventCallbacks {
            [jsOnClick = JSFunctionForUI::Persist(info[3].As<Napi::Function>(), true)]() {
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

    void MenuItemWrap::Destroy(const Napi::CallbackInfo& info) {
        UISyncDelayable(info.Env(), [this]() {
            this->menu_item_.reset();
        });
    }
    //MenuItemWrap Implementations End


    //MenuWrap Implementations Begin

    Napi::Function MenuWrap::Constructor(const Napi::Env& env) {
        return DefineClass(env, "MenuNative", {
            InstanceMethod("append", &MenuWrap::Append),
            InstanceMethod("destroy", &MenuWrap::Destroy)
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
        Napi::Object jsMenuItem = info[0].As<Napi::Object>();
        MenuItemWrap* wrappedMenuItem = MenuItemWrap::Unwrap(jsMenuItem);
        
        UISyncDelayable(info.Env(), [this, wrappedMenuItem] {
            this->menu_->AppendItem(*(wrappedMenuItem->menu_item_)); 
        });
    }

    void MenuWrap::Destroy(const Napi::CallbackInfo& info) {
        UISyncDelayable(info.Env(), [this]() {
            this->menu_.reset();
        });
    }
    //MenuWrap Implementations End

}
