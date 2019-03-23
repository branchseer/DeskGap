#ifndef webview_h
#define webview_h

#include <functional>
#include <memory>
#include "../menu/menu.h"
#include <optional>
#include <string>
#include <vector>
#include <variant>

// On Windows the WebView class is a pure virtual interface,
// because there are 2 implementations at runtime: WinRTWebView (using the WebViewControl in WinRT, Win10 1809+ only)
// and TridentWebView (using IWebBrowser2, Win 7+ supported)
#ifdef WIN32
    #define VIRTUAL_IF_WIN32(decl) virtual decl=0
#else
    #define VIRTUAL_IF_WIN32(decl) decl
#endif

namespace DeskGap {
    class WebView {
    protected:
        friend class BrowserWindow;
        struct Impl;
        std::unique_ptr<Impl> impl_;
    public:
        struct LoadingError {
            long code;
            std::string description;
        };
        struct EventCallbacks {
            std::function<void()> didStartLoading;
            std::function<void(const std::optional<LoadingError>&)> didStopLoading;
            std::function<void(std::string&&)> onStringMessage;
            std::function<void(const std::string&)> onPageTitleUpdated;
        };

        #ifndef WIN32
        WebView(EventCallbacks&&);
        #endif

        struct HTTPHeader {
            std::string field;
            std::string value;
        };

        VIRTUAL_IF_WIN32(void LoadHTMLString(const std::string&));
        VIRTUAL_IF_WIN32(void LoadLocalFile(const std::string& path));
        VIRTUAL_IF_WIN32(void LoadRequest(
            const std::string& method,
            const std::string& urlString,
            const std::vector<HTTPHeader>& headers,
            const std::optional<std::string>& body
        ));
        VIRTUAL_IF_WIN32(void Reload());
        using JavaScriptEvaluationCallback = std::function<void(bool, std::string&&)>;
        VIRTUAL_IF_WIN32(void EvaluateJavaScript(const std::string& scriptString, std::optional<JavaScriptEvaluationCallback>&&));


        VIRTUAL_IF_WIN32(void SetDevToolsEnabled(bool enabled));

        VIRTUAL_IF_WIN32(~WebView());
    };

    //WinRTWebView and TridentWebView have their own Impls which inherit WebView::Impl,
    //the impl_ declared in parent WebView is essentially winrtImpl_ or tridentImpl_,
    //these two are declared in the subclasses for the ease of subclass-specific method access
    #ifdef WIN32
    class WinRTWebView: public WebView {
    private:
        struct Impl;
        Impl* winrtImpl_;
    public:
        WinRTWebView(EventCallbacks&&);
        virtual void LoadHTMLString(const std::string&) override;
        virtual void LoadLocalFile(const std::string& path) override;
        virtual void LoadRequest(
            const std::string& method,
            const std::string& urlString,
            const std::vector<HTTPHeader>& headers,
            const std::optional<std::string>& body
        ) override;
        virtual void Reload() override;
        virtual void SetDevToolsEnabled(bool enabled) override;
        virtual void EvaluateJavaScript(const std::string& scriptString, std::optional<JavaScriptEvaluationCallback>&&) override;
        virtual ~WinRTWebView();
    };
    class TridentWebView: public WebView {
    private:
        struct Impl;
        Impl* tridentImpl_;
    public:
        TridentWebView(EventCallbacks&&);
        virtual void LoadHTMLString(const std::string&) override;
        virtual void LoadLocalFile(const std::string& path) override;
        virtual void LoadRequest(
            const std::string& method,
            const std::string& urlString,
            const std::vector<HTTPHeader>& headers,
            const std::optional<std::string>& body
        ) override;
        virtual void Reload() override;
        virtual void SetDevToolsEnabled(bool enabled) override;
        virtual void EvaluateJavaScript(const std::string& scriptString, std::optional<JavaScriptEvaluationCallback>&&) override;
        virtual ~TridentWebView();
    };
    #endif
}

#endif
