#ifndef mac_webview_impl_h
#define mac_webview_impl_h

#include "../webview/webview.h"
#include <vcclr.h> 

namespace DeskGap {
    struct WebView::Impl {
        gcroot<Microsoft::Toolkit::Forms::UI::Controls::WebView^> uwpWebView;
        void PrepareScript();
    };
}

#endif
