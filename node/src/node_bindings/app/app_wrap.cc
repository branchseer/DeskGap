#include "app_wrap.h"
#include <deskgap/app.hpp>
#include "../dispatch/dispatch.h"
#include "../menu/menu_wrap.h"
#include "../util/js_native_convert.h"
#include "app_startup.hpp"


Napi::Object DeskGap::AppWrap::AppObject(const Napi::Env& env) {
    using namespace JSNativeConvertion;

    Napi::Object appObject = Napi::Object::New(env);
    appObject.Set("run", Napi::Function::New(env, [](const Napi::CallbackInfo& info) {
        Napi::Object jsCallbacks = info[0].As<Napi::Object>();
        auto jsOnReady = JSFunctionForUI::Persist(jsCallbacks.Get("onReady").As<Napi::Function>());
        auto jsBeforeQuit = JSFunctionForUI::Persist(jsCallbacks.Get("beforeQuit").As<Napi::Function>());
        DeskGap::AppStartup::SignalAppRun({
              [jsOnReady { std::move(jsOnReady) }]() {
                  jsOnReady->Call();
              },
              [jsBeforeQuit { std::move(jsBeforeQuit) }]() {
                  jsBeforeQuit->Call();
              }
        });
    }));

    appObject.Set("exit", Napi::Function::New(env, [](const Napi::CallbackInfo& info) {
        uint32_t exitCode = Native<uint32_t>::From(info[0]);
        UISync(info.Env(), [exitCode]() {
            DeskGap::App::Exit(exitCode);
        });
    }));

#ifdef __APPLE__
    appObject.Set("setMenu", Napi::Function::New(env, [](const Napi::CallbackInfo& info) {
        MenuWrap* wrappedMenu = nullptr;
        if (Napi::Value jsValue = info[0]; !jsValue.IsNull()) {
            wrappedMenu = MenuWrap::Unwrap(jsValue.As<Napi::Object>());
        }
        UISyncDelayable(info.Env(), [wrappedMenu] {
            DeskGap::App::SetMenu(
                wrappedMenu == nullptr ? std::nullopt :
                std::make_optional(std::ref(*(wrappedMenu->menu_)))
            );
        });
    }));
#endif

    appObject.Set("getPath", Napi::Function::New(env, [](const Napi::CallbackInfo& info) {
        std::string path = DeskGap::App::GetPath(static_cast<App::PathName>(Native<uint32_t>::From(info[0])));
        return JSFrom(info.Env(), path);
    }));

    appObject.Set("getArgv", Napi::Function::New(env, [](const Napi::CallbackInfo &info) {
        return JSFrom(info.Env(), AppStartup::ExecArgs());
    }));

    appObject.Set("getResourcePath", Napi::Function::New(env, [](const Napi::CallbackInfo &info) {
        return JSFrom(info.Env(), AppStartup::ResourcePath());
    }));
    return appObject;
}

