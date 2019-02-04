#include "app_wrap.h"
#include "app.h"
#include "../dispatch/dispatch.h"
#include "../menu/menu_wrap.h"
#include "../util/js_native_convert.h"

namespace DeskGap {
    using namespace JSNativeConvertion;

        void AppWrap::Run(const Napi::CallbackInfo& info) {
            UISync(info.Env(), [this]() {
                this->app_->Run();
            });
        }

        void AppWrap::Exit(const Napi::CallbackInfo& info) {
            uint32_t exitCode = Native<uint32_t>::From(info[0]);
            UISync(info.Env(), [this, exitCode]() {
                this->app_->Exit(exitCode);
            });
        }

    #ifdef __APPLE__
        void AppWrap::SetMenu(const Napi::CallbackInfo& info) {
            MenuWrap* wrappedMenu = nullptr;
            if (Napi::Value jsValue = info[0]; !jsValue.IsNull()) {
                wrappedMenu = MenuWrap::Unwrap(jsValue.As<Napi::Object>());
            }
            UISyncDelayable(info.Env(), [this, wrappedMenu] {
                this->app_->SetMenu(
                    wrappedMenu == nullptr ? std::nullopt :
                    std::make_optional(std::ref(*(wrappedMenu->menu_)))
                );
            });
        }
    #endif

        Napi::Value AppWrap::GetPath(const Napi::CallbackInfo& info) {
            std::string path = app_->GetPath(static_cast<App::PathName>(Native<uint32_t>::From(info[0])));
            return JSFrom(info.Env(), path);
        }

        AppWrap::AppWrap(const Napi::CallbackInfo& info):
            Napi::ObjectWrap<AppWrap>(info)
        {
            Napi::Object jsCallbacks = info[0].ToObject();
            Value().Set("callbacks_", jsCallbacks);
            
            App::EventCallbacks callbacks {
                [jsOnReady = JSFunctionForUI::Weak(jsCallbacks.Get("onReady").As<Napi::Function>())]() {
                    jsOnReady->Call();
                },
                [jsBeforeQuit = JSFunctionForUI::Weak(jsCallbacks.Get("beforeQuit").As<Napi::Function>())]() {
                    jsBeforeQuit->Call();
                }
            };

            UISync(info.Env(), [&]() {
                app_ = std::make_unique<App>(callbacks);
            });
        }
        Napi::Function AppWrap::Constructor(Napi::Env env) {
            return DefineClass(env, "AppNative", {
                InstanceMethod("run", &AppWrap::Run),
                InstanceMethod("exit", &AppWrap::Exit),
                InstanceMethod("getPath", &AppWrap::GetPath),
            #ifdef __APPLE__
                InstanceMethod("setMenu", &AppWrap::SetMenu),
            #endif
            });
        }
    
}
