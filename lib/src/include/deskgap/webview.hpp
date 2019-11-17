#ifndef DESKGAP_WEBVIEW_HPP
#define DESKGAP_WEBVIEW_HPP

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <vector>

// On Windows the WebView class is a pure virtual interface,
// because there are 2 implementations at runtime: WinRTWebView (using the WebViewControl in WinRT, Win10 1809+ only)
// and TridentWebView (using IWebBrowser2, Win 7+ supported)
#ifdef WIN32
    #pragma warning(disable: 4275)
    #define VIRTUAL_IF_WIN32 virtual
    #define PURE_VIRTUAL_IF_WIN32(decl) virtual decl=0
#else
    #define VIRTUAL_IF_WIN32
    #define PURE_VIRTUAL_IF_WIN32(decl) decl
    
#endif


namespace DeskGap {
    class WebView {
    protected:
        friend class BrowserWindow;
        struct Impl;
        std::unique_ptr<Impl> impl_;
    public:
        #ifdef WIN32
        static bool IsWinRTWebViewAvailable();
        #endif
        struct EventCallbacks {
            std::function<void()> didFinishLoad;
            std::function<void(std::string&&)> onStringMessage;
            std::function<void(const std::string&)> onPageTitleUpdated;
        };

        #ifndef WIN32
        WebView(EventCallbacks&&, const std::string& preloadScriptString);
        #endif

        struct HTTPHeader {
            std::string field;
            std::string value;
        };

        PURE_VIRTUAL_IF_WIN32(void LoadLocalFile(const std::string& path));
        PURE_VIRTUAL_IF_WIN32(void LoadRequest(
            const std::string& method,
            const std::string& urlString,
            const std::vector<HTTPHeader>& headers,
            const std::optional<std::string>& body
        ));
        PURE_VIRTUAL_IF_WIN32(void Reload());
        using JavaScriptExecutionCallback = std::function<void(std::optional<std::string>&&)>; // std::optional<std::string>: error message
        PURE_VIRTUAL_IF_WIN32(void ExecuteJavaScript(const std::string& scriptString, std::optional<JavaScriptExecutionCallback>&&));

        PURE_VIRTUAL_IF_WIN32(void SetDevToolsEnabled(bool enabled));

        #ifdef WIN32
        inline virtual ~WebView() = default;
        #else
        ~WebView();
        #endif
    };
}

#ifdef WIN32
#include "winrt_webview.hpp"
namespace DeskGap {
    class TridentWebView: public WebView {
    private:
        struct Impl;
        Impl* tridentImpl_;
    public:
        TridentWebView(EventCallbacks&&, const std::string& preloadScriptString);
        virtual void LoadLocalFile(const std::string& path) override;
        virtual void LoadRequest(
            const std::string& method,
            const std::string& urlString,
            const std::vector<HTTPHeader>& headers,
            const std::optional<std::string>& body
        ) override;
        virtual void Reload() override;
        virtual void SetDevToolsEnabled(bool enabled) override;
        virtual void ExecuteJavaScript(const std::string& scriptString, std::optional<JavaScriptExecutionCallback>&&) override;
        virtual ~TridentWebView();
    };
}

#endif 

#endif
