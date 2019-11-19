#include "webview.hpp"
#include "webview_impl.h"
#include "./util/winrt.h"

namespace DeskGap {
    bool WebView::IsWinRTWebViewAvailable() {
        if (GetWinRTDLLModule() == nullptr) {
            return false;
        }
        return WinRTWebView::IsAvailable();
    }
}
