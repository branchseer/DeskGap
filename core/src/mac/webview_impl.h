#ifndef mac_webview_impl_h
#define mac_webview_impl_h

#import <WebKit/WebKit.h>
#include "../webview/webview.h"

namespace DeskGap {
    struct WebView::Impl {
        WKWebView* wkWebView;
        NSObject* webViewDelegate;
        NSObject* localURLSchemeHandler;
        void Destroy();
        void ServePath(NSString* path);
    };
}

#endif
