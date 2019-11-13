#ifndef DESKGAP_MAC_WEBVIEW_IMPL_H
#define DESKGAP_MAC_WEBVIEW_IMPL_H

#import <WebKit/WebKit.h>
#include "webview.hpp"

namespace DeskGap {
    struct WebView::Impl {
        WKWebView* wkWebView;
        NSObject* webViewDelegate;
        NSObject* localURLSchemeHandler;
        void ServePath(NSString* path);
        ~Impl();
    };
}

#endif
