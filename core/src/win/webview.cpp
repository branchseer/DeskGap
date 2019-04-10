#include "../webview/webview.h"
#include "webview_impl.h"
#include "./util/winrt.h"

namespace DeskGap {
    bool WebView::IsWinRTWebViewAvaliable() {
        if (GetWinRTDLLModule() == nullptr) {
            return false;
        }
        return WinRTWebView::IsAvaliable();
    }
}
