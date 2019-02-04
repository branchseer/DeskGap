#ifndef webview_webview_wrap_h
#define webview_webview_wrap_h

#include <napi.h>
#include <functional>
#include <memory>
#include "webview.h"

namespace DeskGap {
    class WebViewWrap: public Napi::ObjectWrap<WebViewWrap> {
    private:
        friend class BrowserWindowWrap;
        std::unique_ptr<WebView> webview_;
        void LoadHTMLString(const Napi::CallbackInfo& info);
        void LoadLocalFile(const Napi::CallbackInfo& info);
        void LoadRequest(const Napi::CallbackInfo& info);
        void EvaluateJavaScript(const Napi::CallbackInfo& info);
        void Reload(const Napi::CallbackInfo&);
    #ifdef __APPLE__
        void SetDevToolsEnabled(const Napi::CallbackInfo& info);
    #endif
    
    public:
        WebViewWrap(const Napi::CallbackInfo& info);
        static Napi::Function Constructor(const Napi::Env& env);
    };
}

#endif
