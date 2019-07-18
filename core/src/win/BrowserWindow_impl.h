#ifndef win_browserwindow_impl_h
#define win_browserwindow_impl_h

#include <Windows.h>

#include "../window/browser_window.h"
#include "../webview/webview.h"

namespace DeskGap {
    struct BrowserWindow::Impl {
        HWND windowWnd;
        const WebView& webView;
        BrowserWindow::EventCallbacks callbacks;

        Impl(const WebView& webView, EventCallbacks& callbacks);

        POINT maxTrackSize;
        POINT minTrackSize;

        HANDLE appIcon {nullptr};
        HANDLE windowIcon {nullptr};
    };
}

#endif
