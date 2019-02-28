#ifndef win_webview_impl_h
#define win_webview_impl_h

#include <Windows.h>
#include <objbase.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Web.UI.Interop.h>

#include "../webview/webview.h"

namespace DeskGap {
    struct WebView::Impl {
        HWND controlWnd;
        winrt::Windows::Web::UI::Interop::WebViewControlProcess process;
        winrt::Windows::Web::UI::Interop::WebViewControl webViewControl;

        class StreamResolver;
        std::unique_ptr<StreamResolver> streamResolver;

		Impl();

        void InitControl(HWND parentWnd);
    };
}

#endif
