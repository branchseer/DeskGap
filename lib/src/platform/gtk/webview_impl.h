#ifndef gtk_webview_impl_h
#define gtk_webview_impl_h

#include <optional>
#include <webkit2/webkit2.h>

#include "webview.hpp"

namespace DeskGap {
    struct WebView::Impl {
		WebKitWebView* gtkWebView;
		WebView::EventCallbacks callbacks;
		std::optional<std::string> servedPath;

		static void HandleLocalFileUriSchemeRequest(WebKitURISchemeRequest *request, gpointer);
		
		gulong loadChangedConnection;
		static void HandleLoadChanged(GtkWidget*, WebKitLoadEvent, WebView*);

		gulong buttonPressEventConnection;
		static gboolean HandleButtonPressEvent(GtkWidget*, GdkEventButton*, WebView*);

		gulong buttonReleaseEventConnection;
		static gboolean HandleButtonReleaseEvent(GtkWidget*, GdkEventButton*, WebView*);

		std::optional<GdkEventButton> lastLeftMouseDownEvent;

		gulong scriptWindowDragConnection;
		static void HandleScriptWindowDrag(WebKitUserContentManager*, WebKitJavascriptResult*, WebView*);

		gulong scriptStringMessageConnection;
		static void HandleScriptStringMessage(WebKitUserContentManager*, WebKitJavascriptResult*, WebView*);
		
		gulong titleChangedConnection;
		static void HandleTitleChanged(GObject*, GParamSpec* pspec, WebView*);
    };
}

#endif
