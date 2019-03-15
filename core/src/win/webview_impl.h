#ifndef win_webview_impl_h
#define win_webview_impl_h

#include <Windows.h>
#include "../webview/webview.h"

namespace DeskGap {
    struct WebView::Impl {
        virtual void SetRect(int x, int y, int width, int height) = 0;
        virtual void InitWithParent(HWND parentWnd) = 0;
    };
}

#endif
