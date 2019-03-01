#ifndef win_browserwindow_impl_h
#define win_browserwindow_impl_h

#include <Windows.h>

#include "../window/browser_window.h"
#include "../webview/webview.h"

namespace DeskGap {
    struct BrowserWindow::Impl {
        HWND windowWnd;
        HWND webViewControlWnd;
        BrowserWindow::EventCallbacks callbacks;
    };
}

#endif
