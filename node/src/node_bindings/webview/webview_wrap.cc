#include <memory>
#include <vector>

#include "webview_wrap.h"
#include <deskgap/webview.hpp>
#include "../dispatch/dispatch.h"

extern "C" {
    extern char BIN2CODE_DG_UI_JS_CONTENT[];
    extern int BIN2CODE_DG_UI_JS_SIZE;
#ifdef WIN32
    extern char BIN2CODE_ES6_PROMISE_AUTO_MIN_JS_CONTENT[];
    extern int BIN2CODE_ES6_PROMISE_AUTO_MIN_JS_SIZE;
#endif
}


namespace DeskGap {
    Napi::Function WebViewWrap::Constructor(const Napi::Env& env) {
        return DefineClass(env, "WebViewNative", {
        #ifdef WIN32
            StaticMethod("isWinRTEngineAvailable", &WebViewWrap::IsWinRTEngineAvailable),
        #endif
            InstanceMethod("loadLocalFile", &WebViewWrap::LoadLocalFile),
            InstanceMethod("loadRequest", &WebViewWrap::LoadRequest),
            InstanceMethod("executeJavaScript", &WebViewWrap::ExecuteJavaScript),
            InstanceMethod("reload", &WebViewWrap::Reload),
            InstanceMethod("setDevToolsEnabled", &WebViewWrap::SetDevToolsEnabled),
            InstanceMethod("destroy", &WebViewWrap::Destroy),
        });
    }

    WebViewWrap::WebViewWrap(const Napi::CallbackInfo& info):
            Napi::ObjectWrap<WebViewWrap>(info)
    {
        Napi::Object jsCallbacks = info[0].As<Napi::Object>();

        WebView::EventCallbacks eventCallbacks {
            [jsDidFinishLoad = JSFunctionForUI::Persist(jsCallbacks.Get("didFinishLoad").As<Napi::Function>())]() {
                jsDidFinishLoad->Call();
            },
            [jsOnStringMessage = JSFunctionForUI::Persist(jsCallbacks.Get("onStringMessage").As<Napi::Function>())](std::string&& stringMessage) {
                jsOnStringMessage->Call([stringMessage { std::move(stringMessage) }](auto env) -> std::vector<napi_value> {
                    return { Napi::String::New(env, stringMessage) };
                });
            },
            [jsOnPageTitleUpdated = JSFunctionForUI::Persist(jsCallbacks.Get("onPageTitleUpdated").As<Napi::Function>())](const std::string& title) {
                jsOnPageTitleUpdated->Call([title](auto env) -> std::vector<napi_value> {
                    return { Napi::String::New(env, title) };
                });
            },
        };

    #ifdef WIN32
        Napi::Number engineValue = info[1].As<Napi::Number>();
        Engine engine = static_cast<Engine>(engineValue.Uint32Value());
    #endif

        UISyncDelayable(info.Env(), [
            this,
            eventCallbacks = std::move(eventCallbacks)
        #ifdef WIN32
            , engine
        #endif
        ]() mutable {
            static std::string dgPreloadScript(BIN2CODE_DG_UI_JS_CONTENT, BIN2CODE_DG_UI_JS_SIZE);
        #ifdef WIN32
            static std::string dgPreloadScriptWithPromise =
                    std::string(BIN2CODE_ES6_PROMISE_AUTO_MIN_JS_CONTENT, BIN2CODE_ES6_PROMISE_AUTO_MIN_JS_SIZE) +
                    dgPreloadScript;

            if (engine == Engine::WINRT) {
                this->webview_ = std::make_unique<WinRTWebView>(std::move(eventCallbacks), dgPreloadScript);
            }
            else {
                this->webview_ = std::make_unique<TridentWebView>(std::move(eventCallbacks), dgPreloadScriptWithPromise);
            }
        #else
            this->webview_ = std::make_unique<WebView>(std::move(eventCallbacks), dgPreloadScript);
        #endif

        });
    }
    
    #ifdef WIN32
    Napi::Value WebViewWrap::IsWinRTEngineAvailable(const Napi::CallbackInfo& info) {
        return Napi::Boolean::New(info.Env(), WebView::IsWinRTWebViewAvailable());
    }
    #endif

    void WebViewWrap::LoadLocalFile(const Napi::CallbackInfo& info) {
        UISyncDelayable(info.Env(), [this, path = info[0].As<Napi::String>().Utf8Value()]() {
            this->webview_->LoadLocalFile(path);
        });
    }

    void WebViewWrap::LoadRequest(const Napi::CallbackInfo& info) {
        std::string method = info[0].As<Napi::String>().Utf8Value();
        std::string url = info[1].As<Napi::String>().Utf8Value();
        Napi::Array jsHeaders = info[2].As<Napi::Array>();
        std::vector<WebView::HTTPHeader> headers;

        uint32_t headerCount = jsHeaders.Length();
        headers.reserve(headerCount);

        for (uint32_t i = 0; i < headerCount; ++i) {
            Napi::Array jsHeader = jsHeaders.Get(i).As<Napi::Array>();
            headers.push_back({
                jsHeader.Get((uint32_t)0).As<Napi::String>().Utf8Value(),
                jsHeader.Get((uint32_t)1).As<Napi::String>().Utf8Value()
            });
        }

        std::optional<std::string> body;
        Napi::Value jsBody = info[3];
        if (!jsBody.IsNull()) {
            body = jsBody.As<Napi::String>().Utf8Value();
        }

        UISyncDelayable(info.Env(), [
            this, method, url, headers, body
        ] {
            this->webview_->LoadRequest(method, url, headers, body);
        });
    }
    void WebViewWrap::Reload(const Napi::CallbackInfo& info) {
        UISyncDelayable(info.Env(), [this]() {
            this->webview_->Reload();
        });
    }

    void WebViewWrap::SetDevToolsEnabled(const Napi::CallbackInfo& info) {
        bool enabled = info[0].As<Napi::Boolean>().Value();
        UISyncDelayable(info.Env(), [this, enabled]() {
            this->webview_->SetDevToolsEnabled(enabled);
        });
    }

    void WebViewWrap::ExecuteJavaScript(const Napi::CallbackInfo& info) {
        std::optional<WebView::JavaScriptExecutionCallback> optionalCallback;
        if (Napi::Value secondArg = info[1]; !secondArg.IsNull()) {
            optionalCallback.emplace([
                jsCallback { JSFunctionForUI::Persist(secondArg.As<Napi::Function>()) }
            ](std::optional<std::string>&& errorMessage) {
                jsCallback->Call([errorMessage { std::move(errorMessage) }](auto env) -> std::vector<napi_value>  {
                    if (errorMessage.has_value()) {
                        return { Napi::String::New(env, *errorMessage) };
                    }
                    else {
                        return { Napi::Env(env).Null() };
                    }
                });
            });
        }
        UISyncDelayable(info.Env(), [
            this,
            script { info[0].As<Napi::String>().Utf8Value() },
            optionalCallback { std::move(optionalCallback) }
        ]() mutable {
            this->webview_->ExecuteJavaScript(script, std::move(optionalCallback));
        });
    }

    void WebViewWrap::Destroy(const Napi::CallbackInfo& info) {
        UISyncDelayable(info.Env(), [this]() {
            this->webview_.reset();
        });
    }
}
