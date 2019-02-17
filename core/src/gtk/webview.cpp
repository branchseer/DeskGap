#include "../webview/webview.h"
#include "webview_impl.h"
#include "../lib_path.h"

namespace DeskGap {

    WebView::WebView(const EventCallbacks& callbacks): impl_(std::make_unique<Impl>()) {
       
    }

    void WebView::LoadHTMLString(const std::string& html) {

    }

    void WebView::LoadLocalFile(const std::string& path) {

    }

    void WebView::LoadRequest(
        const std::string& method,
        const std::string& urlString,
        const std::vector<HTTPHeader>& headers,
        const std::optional<std::string>& body
    ) {
        
    }

    void WebView::Reload() {

    }

    void WebView::EvaluateJavaScript(const std::string& scriptString, std::optional<JavaScriptEvaluationCallback>&& optionalCallback) {
        
    }


    WebView::~WebView() {
        
    }
}
