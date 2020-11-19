#include <memory>
#include "browser_window_wrap.h"
#include "../menu/menu_wrap.h"
#include "../webview/webview_wrap.h"
#include <deskgap/browser_window.hpp>
#include "../dispatch/dispatch.h"

namespace DeskGap {
    void BrowserWindowWrap::Show(const Napi::CallbackInfo& info) {
        UISyncDelayable(info.Env(), [this]() {
            this->browser_window_->Show();
        });
    }
    void BrowserWindowWrap::SetSize(const Napi::CallbackInfo& info) {
        int width = info[0].As<Napi::Number>();
        int height = info[1].As<Napi::Number>();
        bool animate = info[2].As<Napi::Boolean>();

        UISyncDelayable(info.Env(), [
            this, width, height, animate
        ] {
            this->browser_window_->SetSize(width, height, animate);
        });
    }

    void BrowserWindowWrap::SetPosition(const Napi::CallbackInfo& info) {
        int x = info[0].As<Napi::Number>();
        int y = info[1].As<Napi::Number>();
        bool animate = info[2].As<Napi::Boolean>();

        UISyncDelayable(info.Env(), [
            this, x, y, animate
        ] {
            this->browser_window_->SetPosition(x, y, animate);
        });
    }

    void BrowserWindowWrap::SetMaximumSize(const Napi::CallbackInfo& info) {
        int width = info[0].As<Napi::Number>();
        int height = info[1].As<Napi::Number>();

        UISyncDelayable(info.Env(), [this, width, height] {
            this->browser_window_->SetMaximumSize(width, height);
        });
    }

    void BrowserWindowWrap::SetMinimumSize(const Napi::CallbackInfo& info) {
        int width = info[0].As<Napi::Number>();
        int height = info[1].As<Napi::Number>();

        UISyncDelayable(info.Env(), [this, width, height] {
            this->browser_window_->SetMinimumSize(width, height);
        });
    }

    void BrowserWindowWrap::SetTitle(const Napi::CallbackInfo& info) {
        UISyncDelayable(info.Env(), [this, utf8title = info[0].As<Napi::String>().Utf8Value()] {
            this->browser_window_->SetTitle(utf8title);
        });
    }

    Napi::Value BrowserWindowWrap::GetSize(const Napi::CallbackInfo& info) {
        std::array<int, 2> size;
        UISync(info.Env(), [this, &size]() {
            size = this->browser_window_->GetSize();
        });
        Napi::Array jsSize = Napi::Array::New(info.Env(), 2);
        jsSize.Set((uint32_t)0, Napi::Number::New(info.Env(), size[0]));
        jsSize.Set((uint32_t)1, Napi::Number::New(info.Env(), size[1]));
        return jsSize;
    }

    Napi::Value BrowserWindowWrap::GetPosition(const Napi::CallbackInfo& info) {
        std::array<int, 2> position;
        UISync(info.Env(), [this, &position]() {
            position = this->browser_window_->GetPosition();
        });
        Napi::Array jsPosition = Napi::Array::New(info.Env(), 2);
        jsPosition.Set((uint32_t)0, Napi::Number::New(info.Env(), position[0]));
        jsPosition.Set((uint32_t)1, Napi::Number::New(info.Env(), position[1]));
        return jsPosition;
    }

    void BrowserWindowWrap::Center(const Napi::CallbackInfo& info) {
        UISyncDelayable(info.Env(), [this] {
            this->browser_window_->Center();
        });
    }

    void BrowserWindowWrap::Destroy(const Napi::CallbackInfo& info) {
        UISyncDelayable(info.Env(), [this] { 
            this->browser_window_->Destroy();
            this->browser_window_.reset();
        });
    }

    void BrowserWindowWrap::Close(const Napi::CallbackInfo& info) {
        UISyncDelayable(info.Env(), [this] { this->browser_window_->Close(); });
    }

    void BrowserWindowWrap::PopupMenu(const Napi::CallbackInfo& info) {
        MenuWrap* menuWrap = MenuWrap::Unwrap(info[0].As<Napi::Object>());
        std::array<int, 2> location;

        Napi::Value jsLocation = info[1];
        bool hasLocation = !jsLocation.IsNull();
        if (hasLocation) {
            Napi::Array jsArrayLocation = jsLocation.As<Napi::Array>();
            location = {
                jsArrayLocation.Get((uint32_t)0).As<Napi::Number>(),
                jsArrayLocation.Get((uint32_t)1).As<Napi::Number>()
            };
        }
        int positioningItem = info[2].As<Napi::Number>();
        std::function<void()> onClose = [jsOnClose = JSFunctionForUI::Persist(info[3].As<Napi::Function>())]() {
            jsOnClose->Call();
        };
        UISyncDelayable(info.Env(), [this, menuWrap, hasLocation, location, positioningItem, onClose = std::move(onClose)]() mutable {
            this->browser_window_->PopupMenu(*(menuWrap->menu_), hasLocation ? &location: nullptr, positioningItem, std::move(onClose));
        });
    }

#ifndef __APPLE__
    void BrowserWindowWrap::SetMenu(const Napi::CallbackInfo& info) {
        MenuWrap* menuWrap = nullptr;
        if (!info[0].IsNull()) {
            menuWrap = MenuWrap::Unwrap(info[0].As<Napi::Object>());
        }
        UISyncDelayable(info.Env(), [this, menuWrap] {
            this->browser_window_->SetMenu((menuWrap == nullptr) ? nullptr: menuWrap->menu_.get());
        });
    }
    void BrowserWindowWrap::SetIcon(const Napi::CallbackInfo& info) {
        Napi::Value jsIconPath = info[0];
        std::optional<std::string> iconPath;
        if (!jsIconPath.IsNull()) {
            iconPath = jsIconPath.As<Napi::String>().Utf8Value();
        }
        UISyncDelayable(info.Env(), [this, iconPath] {
            this->browser_window_->SetIcon(iconPath);
        });
    }
#endif

#ifdef __APPLE__
    void BrowserWindowWrap::SetTitleBarStyle(const Napi::CallbackInfo& info) {
        auto titleBarStyle = static_cast<BrowserWindow::TitleBarStyle>(info[0].As<Napi::Number>().Int32Value());
        UISyncDelayable(info.Env(), [this, titleBarStyle] {
            this->browser_window_->SetTitleBarStyle(titleBarStyle);
        });
    }

    void BrowserWindowWrap::SetVibrancies(const Napi::CallbackInfo& info) {
        using Vibrancy = BrowserWindow::Vibrancy;
        using Constraint = Vibrancy::Constraint;
        using Unit = Constraint::Unit;

        Napi::Array jsVibrancies = info[0].As<Napi::Array>();
        uint32_t vCount = jsVibrancies.Length();

        std::vector<Vibrancy> vibrancies;
        for (uint32_t iv = 0; iv < vCount; ++iv) {
            Napi::Array jsVibrancy = jsVibrancies.Get(iv).As<Napi::Array>();

            BrowserWindow::Vibrancy v {
                jsVibrancy.Get((uint32_t)0).As<Napi::String>(), //material
                jsVibrancy.Get((uint32_t)1).As<Napi::String>(), //blendingMode
                jsVibrancy.Get((uint32_t)2).As<Napi::String>(), //state
                { }
            };

            Napi::Array jsConstraints = jsVibrancy.Get((uint32_t)3).As<Napi::Array>();
            for (uint32_t ic = 0; ic < 4; ++ic) {
                Napi::Array jsConstraint = jsConstraints.Get((uint32_t)ic).As<Napi::Array>();

                v.constraints[ic] = {
                    jsConstraint.Get((uint32_t)0).As<Napi::String>(), //attribute
                    jsConstraint.Get((uint32_t)1).As<Napi::Number>(), //value
                    jsConstraint.Get((uint32_t)2).As<Napi::Boolean>().Value() ? //unit
                        Unit::POINT : Unit::PERCENTAGE
                };
            }

            vibrancies.push_back(v);
        }

        UISyncDelayable(info.Env(), [this, vibrancies] {
            this->browser_window_->SetVibrancies(vibrancies);
        });
    }
#endif

    void BrowserWindowWrap::SetMaximizable(const Napi::CallbackInfo& info) {
        UISyncDelayable(info.Env(), [this, maximizable = info[0].As<Napi::Boolean>().Value()] {
            this->browser_window_->SetMaximizable(maximizable);
        });
    }
    void BrowserWindowWrap::SetMinimizable(const Napi::CallbackInfo& info) {
        UISyncDelayable(info.Env(), [this, minimizable = info[0].As<Napi::Boolean>().Value()] {
            this->browser_window_->SetMinimizable(minimizable);
        });
    }
    void BrowserWindowWrap::SetResizable(const Napi::CallbackInfo& info) {
        UISyncDelayable(info.Env(), [this, resizable = info[0].As<Napi::Boolean>().Value()] {
            this->browser_window_->SetResizable(resizable);
        });
    }
    void BrowserWindowWrap::SetHasFrame(const Napi::CallbackInfo& info) {
        UISyncDelayable(info.Env(), [this, hasFrame = info[0].As<Napi::Boolean>().Value()] {
            this->browser_window_->SetHasFrame(hasFrame);
        });
    }
    void BrowserWindowWrap::SetClosable(const Napi::CallbackInfo& info) {
        UISyncDelayable(info.Env(), [this, closable = info[0].As<Napi::Boolean>().Value()] {
            this->browser_window_->SetClosable(closable);
        });
    }

    void BrowserWindowWrap::Minimize(const Napi::CallbackInfo& info) {
        UISyncDelayable(info.Env(), [this] {
            this->browser_window_->Minimize();
        });
    }

    BrowserWindowWrap::BrowserWindowWrap(const Napi::CallbackInfo& info):
        Napi::ObjectWrap<BrowserWindowWrap>(info)
    {
        WebViewWrap* webViewWrap = WebViewWrap::Unwrap(info[0].As<Napi::Object>());
        Napi::Object jsCallbacks = info[1].As<Napi::Object>();

        BrowserWindow::EventCallbacks callbacks {
            [jsOnBlur = JSFunctionForUI::Persist(jsCallbacks.Get("onBlur").As<Napi::Function>())]() {
                jsOnBlur->Call();
            },
            [jsOnFocus = JSFunctionForUI::Persist(jsCallbacks.Get("onFocus").As<Napi::Function>())]() {
                jsOnFocus->Call();
            },
            [jsOnResize = JSFunctionForUI::Persist(jsCallbacks.Get("onResize").As<Napi::Function>())]() {
                jsOnResize->Call();
            },
            [jsOnMove = JSFunctionForUI::Persist(jsCallbacks.Get("onMove").As<Napi::Function>())]() {
                jsOnMove->Call();
            },
            [jsOnClose = JSFunctionForUI::Persist(jsCallbacks.Get("onClose").As<Napi::Function>())]() {
                jsOnClose->Call();
            }
#ifdef __APPLE__
            //TODO: Export fullscreen events to js
            ,[]() {}, [](){}, [](){}, [](){}
#endif
        };
        UISyncDelayable(info.Env(), [this, webViewWrap, callbacks = std::move(callbacks)]() mutable {
            this->browser_window_ = std::make_unique<BrowserWindow>(*(webViewWrap->webview_), std::move(callbacks));
        });
    }
    Napi::Function BrowserWindowWrap::Constructor(Napi::Env env) {
        return DefineClass(env, "BrowserWindowNative", {
            InstanceMethod("show", &BrowserWindowWrap::Show),
            InstanceMethod("setSize", &BrowserWindowWrap::SetSize),
            InstanceMethod("setMaximumSize", &BrowserWindowWrap::SetMaximumSize),
            InstanceMethod("setMinimumSize", &BrowserWindowWrap::SetMinimumSize),
            InstanceMethod("setPosition", &BrowserWindowWrap::SetPosition),
            InstanceMethod("setTitle", &BrowserWindowWrap::SetTitle),
            InstanceMethod("center", &BrowserWindowWrap::Center),
            InstanceMethod("getPosition", &BrowserWindowWrap::GetPosition),
            InstanceMethod("getSize", &BrowserWindowWrap::GetSize),
            InstanceMethod("destroy", &BrowserWindowWrap::Destroy),
            InstanceMethod("close", &BrowserWindowWrap::Close),
        #ifndef __APPLE__
            InstanceMethod("setMenu", &BrowserWindowWrap::SetMenu),
            InstanceMethod("setIcon", &BrowserWindowWrap::SetIcon),
        #endif
        #ifdef __APPLE__
            InstanceMethod("setTitleBarStyle", &BrowserWindowWrap::SetTitleBarStyle),
            InstanceMethod("setVibrancies", &BrowserWindowWrap::SetVibrancies),
        #endif
            InstanceMethod("popupMenu", &BrowserWindowWrap::PopupMenu),
            InstanceMethod("setMaximizable", &BrowserWindowWrap::SetMaximizable),
            InstanceMethod("setMinimizable", &BrowserWindowWrap::SetMinimizable),
            InstanceMethod("setResizable", &BrowserWindowWrap::SetResizable),
            InstanceMethod("setHasFrame", &BrowserWindowWrap::SetHasFrame),
            InstanceMethod("setClosable", &BrowserWindowWrap::SetClosable),
            InstanceMethod("minimize", &BrowserWindowWrap::Minimize),
        });
    }
    std::reference_wrapper<BrowserWindow> BrowserWindowWrap::UnderlyingObject() {
        return *browser_window_;
    }
}
