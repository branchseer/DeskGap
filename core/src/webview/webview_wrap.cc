#include <memory>
#include <vector>

#include "webview_wrap.h"
#include "webview.h"
#include "../dispatch/dispatch.h"

namespace DeskGap {
    Napi::Function WebViewWrap::Constructor(const Napi::Env& env) {
        return DefineClass(env, "WebViewNative", {
            InstanceMethod("loadHTMLString", &WebViewWrap::LoadHTMLString),
            InstanceMethod("loadLocalFile", &WebViewWrap::LoadLocalFile),
            InstanceMethod("loadRequest", &WebViewWrap::LoadRequest),
            InstanceMethod("evaluateJavaScript", &WebViewWrap::EvaluateJavaScript),
            InstanceMethod("reload", &WebViewWrap::Reload),
            InstanceMethod("setDevToolsEnabled", &WebViewWrap::SetDevToolsEnabled),
            InstanceMethod("destroy", &WebViewWrap::Destroy),
        });
    }

    WebViewWrap::WebViewWrap(const Napi::CallbackInfo& info):
            Napi::ObjectWrap<WebViewWrap>(info)
    {
        Napi::Object jsCallbacks = info[0].ToObject();
        Value().Set("callbacks_", jsCallbacks);

        WebView::EventCallbacks eventCallbacks {
            [jsDidStartLoading = JSFunctionForUI::Weak(jsCallbacks.Get("didStartLoading").As<Napi::Function>())]() {
                jsDidStartLoading->Call();
            },
            [jsDidStopLoading = JSFunctionForUI::Weak(jsCallbacks.Get("didStopLoading").As<Napi::Function>())](const std::optional<WebView::LoadingError>& error) {
                jsDidStopLoading->Call([error](auto env) -> std::vector<napi_value> {
                    std::vector<napi_value> args;
                    if (error.has_value()) {
                        args = {
                            Napi::Number::New(env, error->code),
                            Napi::String::New(env, error->description),
                        };
                    }
                    return args;
                });
            },
            [jsOnStringMessage = JSFunctionForUI::Weak(jsCallbacks.Get("onStringMessage").As<Napi::Function>())](std::string&& stringMessage) {
                jsOnStringMessage->Call([stringMessage { std::move(stringMessage) }](auto env) -> std::vector<napi_value> {
                    return { Napi::String::New(env, stringMessage) };
                });
            },
            [jsOnPageTitleUpdated = JSFunctionForUI::Weak(jsCallbacks.Get("onPageTitleUpdated").As<Napi::Function>())](const std::string& title) {
                jsOnPageTitleUpdated->Call([title](auto env) -> std::vector<napi_value> {
                    return { Napi::String::New(env, title) };
                });
            },
        };
        UISyncDelayable(info.Env(), [this, eventCallbacks = std::move(eventCallbacks)]() mutable {
            this->webview_ = std::make_unique<WebView>(std::move(eventCallbacks));
        });
    }

    void WebViewWrap::LoadHTMLString(const Napi::CallbackInfo& info) {
        UISyncDelayable(info.Env(), [this, html = info[0].As<Napi::String>().Utf8Value()]() {
            this->webview_->LoadHTMLString(html);
        });
    }
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

    void WebViewWrap::EvaluateJavaScript(const Napi::CallbackInfo& info) {
        std::optional<WebView::JavaScriptEvaluationCallback> optionalCallback;
        if (Napi::Value secondArg = info[1]; !secondArg.IsNull()) {
            optionalCallback.emplace([
                jsCallback { JSFunctionForUI::Persist(secondArg.As<Napi::Function>()) }
            ](bool hasError, std::string&& result) {
                jsCallback->Call([hasError, result { std::move(result) }](auto env) -> std::vector<napi_value>  {
                    return { Napi::Boolean::New(env, hasError), Napi::String::New(env, result) };
                });
            });
        }
        UISyncDelayable(info.Env(), [
            this,
            script { info[0].As<Napi::String>().Utf8Value() },
            optionalCallback { std::move(optionalCallback) }
        ]() mutable {
            this->webview_->EvaluateJavaScript(script, std::move(optionalCallback));
        });
    }

    void WebViewWrap::Destroy(const Napi::CallbackInfo& info) {
        UISyncDelayable(info.Env(), [this]() {
            this->webview_.reset();
        });
    }
}
