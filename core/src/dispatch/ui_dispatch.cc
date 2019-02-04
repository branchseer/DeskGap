#include <queue>
#include <functional>
#include <napi.h>
#include <utility>
#include "ui_dispatch.h"
#include "ui_dispatch_platform.h"
#include "../native_exception.h"
#include "node_dispatch.h"

namespace {
    using namespace DeskGap;
    bool shouldUISyncDispatchesBeDelayed = false;
    std::queue<std::function<void()>> delayedUISyncActions;

    Napi::Value NativeExceptionToJSError(napi_env env, const PlatformException& exception) {
        return NativeExceptionConstructor().New({
            Napi::String::New(env, exception.name),
            Napi::String::New(env, exception.message),
        });
    }
}

void DeskGap::DelayUISync() {
    shouldUISyncDispatchesBeDelayed = true;
}
void DeskGap::CommitUISync(napi_env env) {
    shouldUISyncDispatchesBeDelayed = false;
    UISync(env, []() {
        while (!delayedUISyncActions.empty()) {
            delayedUISyncActions.front()();
            delayedUISyncActions.pop();
        }
    });
}

void DeskGap::UISyncDelayable(napi_env env, std::function<void()>&& action) {
    if (shouldUISyncDispatchesBeDelayed) {
        delayedUISyncActions.push(std::move(action));
    }
    else {
        UISync(env, std::move(action));
    }
}

void DeskGap::UISync(napi_env env, std::function<void()>&& action) {
    std::optional<PlatformException> optionalException = DeskGap::UISyncPlatform(std::move(action));
    if (optionalException.has_value()) {
        throw NativeExceptionToJSError(env, *optionalException).As<Napi::Error>();
    }
}

void DeskGap::UIASync(napi_env env, std::function<void()>&& action) {
    auto asyncThrowJSError = JSFunctionForUI::Persist(Napi::Function::New(env, [](const Napi::CallbackInfo& info) {
        throw info[0].As<Napi::Error>();
    }));
    UIASyncPlatform(std::move(action), [asyncThrowJSError = std::move(asyncThrowJSError)](std::optional<PlatformException>&& optionalException) {
        if (optionalException.has_value()) {
            asyncThrowJSError->Call([exception = std::move(*optionalException), asyncThrowJSError](napi_env env) {
                return std::vector<napi_value> {
                    NativeExceptionToJSError(env, exception)
                };
            });
        }
    });
}
