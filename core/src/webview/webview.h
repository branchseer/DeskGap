#ifndef webview_h
#define webview_h

#include <functional>
#include <memory>
#include "../menu/menu.h"
#include <optional>
#include <string>
#include <vector>
#include <variant>

namespace DeskGap {
    class WebView {
    private:
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
        WebView(EventCallbacks&&);

        struct HTTPHeader {
            std::string field;
            std::string value;
        };

        void LoadHTMLString(const std::string&);
        void LoadLocalFile(const std::string& path);
        void LoadRequest(
            const std::string& method,
            const std::string& urlString,
            const std::vector<HTTPHeader>& headers,
            const std::optional<std::string>& body
        );
        void Reload();
        using JavaScriptEvaluationCallback = std::function<void(bool, std::string&&)>;
        void EvaluateJavaScript(const std::string& scriptString, std::optional<JavaScriptEvaluationCallback>&&);


    #ifdef __APPLE__
        void SetDevToolsEnabled(bool enabled);
    #endif

        ~WebView();
    };
}

#endif
