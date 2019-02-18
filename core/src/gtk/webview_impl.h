#ifndef gtk_webview_impl_h
#define gtk_webview_impl_h

#include <webkit2/webkit2.h>

#include "../webview/webview.h"

namespace DeskGap {
    struct WebView::Impl {
		WebKitWebView* gtkWebView;
		Impl(WebView::EventCallbacks&);
		~Impl();
    };
}

#endif
