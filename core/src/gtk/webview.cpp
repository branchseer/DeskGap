#include "../webview/webview.h"
#include "webview_impl.h"
#include "../lib_path.h"

namespace DeskGap {

    WebView::Impl::Impl(EventCallbacks& callbacks) {
        gtkWebView = WEBKIT_WEB_VIEW(webkit_web_view_new());
    }

    WebView::Impl::~Impl() {
        gtk_widget_destroy((GtkWidget*)gtkWebView);
    }

    WebView::WebView(EventCallbacks&& callbacks): impl_(std::make_unique<Impl>(callbacks)) {
       webkit_web_view_load_html(impl_->gtkWebView, "<h1>Hello WebKitGTK</h1>", nullptr);
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
