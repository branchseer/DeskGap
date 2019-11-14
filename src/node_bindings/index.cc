#include <memory>
#include <sstream>
#include <napi.h>
#include "index.hpp"
#include "app/app_wrap.h"
#include "window/browser_window_wrap.h"
#include "menu/menu_wrap.h"
#include "shell/shell_wrap.h"
#include "webview/webview_wrap.h"
#include "system_preferences/system_preferences_wrap.h"
#include "dialog/dialog_wrap.h"
#include "dispatch/dispatch.h"
#include "native_exception.h"

namespace {
    std::unique_ptr<Napi::FunctionReference> nativeExceptionConstructor;

    inline void ExportFunction(Napi::Object& exports, const Napi::Function& function) {
        exports.Set(function.Get("name"), function);
    }
}


Napi::Object DeskGap::InitNodeNativeModule(Napi::Env env, Napi::Object exports) {
    ExportFunction(exports, DeskGap::AppWrap::Constructor(env));
    ExportFunction(exports, DeskGap::BrowserWindowWrap::Constructor(env));
    ExportFunction(exports, DeskGap::MenuWrap::Constructor(env));
    ExportFunction(exports, DeskGap::MenuItemWrap::Constructor(env));
    ExportFunction(exports, DeskGap::WebViewWrap::Constructor(env));

    ExportFunction(exports, Napi::Function::New(env, [](const Napi::CallbackInfo&) {
        DeskGap::DelayUISync();
    }, "delayUISync"));

    ExportFunction(exports, Napi::Function::New(env, [](const Napi::CallbackInfo& info) {
        DeskGap::CommitUISync(info.Env());
    }, "commitUISync"));

    ExportFunction(exports, Napi::Function::New(env, [](const Napi::CallbackInfo& info) {

    }, "setLibPath"));


    ExportFunction(exports, Napi::Function::New(env, [](const Napi::CallbackInfo& info) {
        nativeExceptionConstructor = std::make_unique<Napi::FunctionReference>(Persistent(info[0].As<Napi::Function>()));
    }, "setNativeExceptionConstructor"));

    exports.Set("shellNative", DeskGap::ShellObject(env));
    exports.Set("systemPreferencesNative", DeskGap::SystemPreferencesObject(env));
    exports.Set("dialogNative", DeskGap::DialogObject(env));

    return exports;
}

const Napi::FunctionReference& DeskGap::NativeExceptionConstructor() {
    return *nativeExceptionConstructor;
}

