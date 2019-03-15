#include "../webview/webview.h"
#include "webview_impl.h"
#include "../lib_path.h"


namespace DeskGap {

    // void WebView::Impl::SetRect(int x, int y, int width, int height) = 0;
    // void WebView::Impl::InitWithParent(HWND parentWnd) = 0;

    // WebView::WebView(EventCallbacks&& callbacks) {
        
    // }
    // void WebView::LoadHTMLString(const std::string& html) = 0;

    // void WebView::LoadLocalFile(const std::string& path) = 0;

    // void WebView::LoadRequest(
    //     const std::string& method,
    //     const std::string& urlString,
    //     const std::vector<HTTPHeader>& headers,
    //     const std::optional<std::string>& body
    // ) = 0;

    // void WebView::EvaluateJavaScript(const std::string& scriptString, std::optional<JavaScriptEvaluationCallback>&& optionalCallback) = 0;

    // void WebView::SetDevToolsEnabled(bool enabled) = 0;

    // void WebView::Reload() = 0;
    // WebView::WebView(EventCallbacks&&) { };
    WebView::~WebView() = default;
    WebView::Impl::~Impl() = default;
}
