#ifndef WINRT_WEBVIEW_H
#define WINRT_WEBVIEW_H

#ifdef _EXPORTING
   #define DECLSPEC __declspec(dllexport)
#else
   #define DECLSPEC __declspec(dllimport)
#endif

#include "webview.h"
#include <string>

namespace DeskGap {
    class DECLSPEC WinRTWebView: public WebView {
    private:
        struct Impl;
        Impl* winrtImpl_;
    public:
        WinRTWebView(EventCallbacks&&, const std::string& libPath);
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
        virtual void ExecuteJavaScript(const std::string& scriptString, std::optional<JavaScriptExecutionCallback>&&) override;
        virtual ~WinRTWebView();
    };
}

#endif
