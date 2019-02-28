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

        winrt::Windows::Web::UI::Interop::WebViewControl::NavigationCompleted_revoker navigationCompletedRevoker;
        winrt::Windows::Web::UI::Interop::WebViewControl::NavigationStarting_revoker navigationStartingRevoker;
        winrt::Windows::Web::UI::Interop::WebViewControl::ScriptNotify_revoker scriptNotifyRevoker;

        class StreamResolver;
        std::unique_ptr<StreamResolver> streamResolver;

        WebView::EventCallbacks callbacks;

		Impl();

        void PrepareScript();
        void InitControl(HWND parentWnd);
    };
}

#endif
