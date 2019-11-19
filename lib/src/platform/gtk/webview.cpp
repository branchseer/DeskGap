#include <filesystem>
#include <unordered_set>
#include <cstring>
#include <gtk/gtk.h>

#include "webview.hpp"
#include "webview_impl.h"
#include "../../utils/mime.hpp"
#include "./glib_exception.h"
#include "./util/convert_js_result.h"

extern "C" {
    extern char BIN2CODE_DG_PRELOAD_GTK_JS_CONTENT[];
    extern int BIN2CODE_DG_PRELOAD_GTK_JS_SIZE;
}
namespace fs = std::filesystem;

namespace {
    const gchar* localURLScheme = "deskgap-local";
    gboolean HandleContextMenu(WebKitWebView*, WebKitContextMenu *menu, GdkEvent*, WebKitHitTestResult*, gpointer) {
        static const std::unordered_set<WebKitContextMenuAction> kActionsToBeDeleted {
            WEBKIT_CONTEXT_MENU_ACTION_OPEN_LINK,
            WEBKIT_CONTEXT_MENU_ACTION_OPEN_LINK_IN_NEW_WINDOW,
            WEBKIT_CONTEXT_MENU_ACTION_DOWNLOAD_LINK_TO_DISK,
            WEBKIT_CONTEXT_MENU_ACTION_COPY_LINK_TO_CLIPBOARD,
            WEBKIT_CONTEXT_MENU_ACTION_OPEN_IMAGE_IN_NEW_WINDOW,
            WEBKIT_CONTEXT_MENU_ACTION_DOWNLOAD_IMAGE_TO_DISK,
            WEBKIT_CONTEXT_MENU_ACTION_COPY_IMAGE_TO_CLIPBOARD,
            WEBKIT_CONTEXT_MENU_ACTION_COPY_IMAGE_URL_TO_CLIPBOARD,
            WEBKIT_CONTEXT_MENU_ACTION_OPEN_FRAME_IN_NEW_WINDOW,
            WEBKIT_CONTEXT_MENU_ACTION_GO_BACK,
            WEBKIT_CONTEXT_MENU_ACTION_GO_FORWARD,
            WEBKIT_CONTEXT_MENU_ACTION_STOP,
            WEBKIT_CONTEXT_MENU_ACTION_RELOAD,
            WEBKIT_CONTEXT_MENU_ACTION_OPEN_VIDEO_IN_NEW_WINDOW,
            WEBKIT_CONTEXT_MENU_ACTION_OPEN_AUDIO_IN_NEW_WINDOW,
            WEBKIT_CONTEXT_MENU_ACTION_COPY_VIDEO_LINK_TO_CLIPBOARD,
            WEBKIT_CONTEXT_MENU_ACTION_COPY_AUDIO_LINK_TO_CLIPBOARD,
            WEBKIT_CONTEXT_MENU_ACTION_DOWNLOAD_VIDEO_TO_DISK,
            WEBKIT_CONTEXT_MENU_ACTION_DOWNLOAD_AUDIO_TO_DISK,
        };

        guint itemCount = webkit_context_menu_get_n_items(menu);
        for (gint i = itemCount - 1; i >= 0; --i) {
            WebKitContextMenuItem* item = webkit_context_menu_get_item_at_position(menu, i);
            WebKitContextMenuAction action = webkit_context_menu_item_get_stock_action(item);
            if (kActionsToBeDeleted.find(action) != kActionsToBeDeleted.end()) {
                webkit_context_menu_remove(menu, item);
            }
        }
        if (webkit_context_menu_get_n_items(menu) == 0) {
            return TRUE;
        }
        return FALSE;
    }
}

namespace DeskGap {

    void WebView::Impl::HandleLocalFileUriSchemeRequest(WebKitURISchemeRequest *request, gpointer webView) {
        const auto& servedPath = static_cast<WebView*>(webView)->impl_->servedPath;
        if (!servedPath.has_value()) {
            GError *error = g_error_new(WEBKIT_NETWORK_ERROR, 404, "Requesting Local Files Not Allowed");
            webkit_uri_scheme_request_finish_error (request, error);
            g_error_free(error);
            return;
        }
        
        const gchar* urlPath = webkit_uri_scheme_request_get_path(request);

        const gchar* encodedFilename = urlPath;
        while (*encodedFilename == '/') ++encodedFilename;

        gchar* fileContent;
        gsize fileSize;
        std::string fileExtension;
        {
            gchar* fullPath;
            {
                gchar* filename = g_uri_unescape_string(encodedFilename, nullptr);

                fullPath = g_build_filename(servedPath.value().c_str(), filename, nullptr);

                if (const char* firstDot = std::strrchr(filename, '.'); firstDot != nullptr) {
                    fileExtension = std::string(firstDot + 1);
                }
                g_free(filename);
            }
            GError* error = nullptr;
            g_file_get_contents(fullPath, &fileContent, &fileSize, &error);

            if (error != nullptr) {
                webkit_uri_scheme_request_finish_error(request, error);
                g_error_free(error);
                return;
            }
        }

        GInputStream *stream = g_memory_input_stream_new_from_data(fileContent, fileSize, g_free);
        webkit_uri_scheme_request_finish(request, stream, fileSize, DeskGap::GetMimeTypeOfExtension(fileExtension).c_str());
        g_object_unref(stream);
    }


    void WebView::Impl::HandleLoadChanged(GtkWidget*, WebKitLoadEvent loadEvent, WebView* webView) {
        switch (loadEvent) {
        case WEBKIT_LOAD_FINISHED:
            webView->impl_->callbacks.didFinishLoad();
            break;
        default:
            break;
        }
    }

    WebView::WebView(EventCallbacks&& callbacks, const std::string& preloadScriptString): impl_(std::make_unique<Impl>()) {
        impl_->callbacks = std::move(callbacks);
        {
            WebKitWebContext* context = webkit_web_context_new();
            webkit_web_context_register_uri_scheme(
                context,
                localURLScheme, Impl::HandleLocalFileUriSchemeRequest,
                this, nullptr
            );

            impl_->gtkWebView = WEBKIT_WEB_VIEW(g_object_ref_sink(webkit_web_view_new_with_context(context)));
            g_object_unref(context);
        }

        {
            WebKitSettings* settings = webkit_web_view_get_settings(impl_->gtkWebView);
            webkit_settings_set_javascript_can_access_clipboard(settings, true);
        }


        std::string preloadScript(BIN2CODE_DG_PRELOAD_GTK_JS_CONTENT, BIN2CODE_DG_PRELOAD_GTK_JS_SIZE);
        preloadScript.reserve(BIN2CODE_DG_PRELOAD_GTK_JS_SIZE);
        preloadScript.assign(BIN2CODE_DG_PRELOAD_GTK_JS_CONTENT, BIN2CODE_DG_PRELOAD_GTK_JS_SIZE);
        preloadScript.append(preloadScriptString);
        {
            WebKitUserContentManager* manager = webkit_web_view_get_user_content_manager(impl_->gtkWebView);

            impl_->scriptWindowDragConnection = g_signal_connect(
                manager,
                "script-message-received::windowDrag",
                G_CALLBACK(Impl::HandleScriptWindowDrag),
                this
            );
            webkit_user_content_manager_register_script_message_handler(manager, "windowDrag");

            impl_->scriptStringMessageConnection = g_signal_connect(
                manager,
                "script-message-received::stringMessage",
                G_CALLBACK(Impl::HandleScriptStringMessage),
                this
            );
            webkit_user_content_manager_register_script_message_handler(manager, "stringMessage");

            {
                WebKitUserScript* preloadUserScript = webkit_user_script_new(
                    preloadScript.c_str(),
                    WEBKIT_USER_CONTENT_INJECT_TOP_FRAME,
                    WEBKIT_USER_SCRIPT_INJECT_AT_DOCUMENT_START,
                    nullptr, nullptr
                );
                webkit_user_content_manager_add_script(manager, preloadUserScript);
                webkit_user_script_unref(preloadUserScript);
            }
        }

        gtk_widget_show(GTK_WIDGET(impl_->gtkWebView));

        g_signal_connect(impl_->gtkWebView, "context-menu", G_CALLBACK(HandleContextMenu), nullptr);

        impl_->loadChangedConnection = g_signal_connect(
            impl_->gtkWebView, "load-changed", 
            G_CALLBACK(Impl::HandleLoadChanged), this
        );
        impl_->buttonPressEventConnection = g_signal_connect(
            impl_->gtkWebView, "button-press-event",
            G_CALLBACK(Impl::HandleButtonPressEvent), this
        );
        impl_->buttonReleaseEventConnection = g_signal_connect(
            impl_->gtkWebView, "button-release-event",
            G_CALLBACK(Impl::HandleButtonReleaseEvent), this
        );
        impl_->titleChangedConnection = g_signal_connect(
            impl_->gtkWebView, "notify::title",
            G_CALLBACK(Impl::HandleTitleChanged), this
        );
    }

    void WebView::Impl::HandleTitleChanged(GObject*, GParamSpec*, WebView* webView) {
        webView->impl_->callbacks.onPageTitleUpdated(
            webkit_web_view_get_title(webView->impl_->gtkWebView)
        );
    }

    void WebView::Impl::HandleScriptWindowDrag(WebKitUserContentManager*, WebKitJavascriptResult*, WebView* webView) {
        std::optional<GdkEventButton>& lastLeftMouseDownEvent = webView->impl_->lastLeftMouseDownEvent;
        if (!lastLeftMouseDownEvent.has_value()) return;

        GtkWidget* window = gtk_widget_get_toplevel(GTK_WIDGET(webView->impl_->gtkWebView));
        if (!GTK_IS_WINDOW(window)) return;

        gtk_window_begin_move_drag(GTK_WINDOW(window),
            lastLeftMouseDownEvent->button,
            lastLeftMouseDownEvent->x_root, lastLeftMouseDownEvent->y_root,
            lastLeftMouseDownEvent->time
        );
        lastLeftMouseDownEvent.reset();
    }
    void WebView::Impl::HandleScriptStringMessage(WebKitUserContentManager*, WebKitJavascriptResult* jsResult, WebView* webView) {
        std::optional<std::string> resultMessage = std::move(jsResultToString(jsResult));
        webkit_javascript_result_unref(jsResult);

        webView->impl_->callbacks.onStringMessage(std::move(*resultMessage));
    }
    gboolean WebView::Impl::HandleButtonPressEvent(GtkWidget*, GdkEventButton* event, WebView* webView) {
        if (event->button == 1 && event->type == GDK_BUTTON_PRESS) {
            webView->impl_->lastLeftMouseDownEvent.emplace(*event);
        }
        return FALSE;
    }
    gboolean WebView::Impl::HandleButtonReleaseEvent(GtkWidget*, GdkEventButton* event, WebView* webView) {
        if (event->button == 1 && event->type == GDK_BUTTON_RELEASE) {
            webView->impl_->lastLeftMouseDownEvent.reset();
        }
        return FALSE;
    }

    WebView::~WebView() {
        for (gulong connection: { 
            impl_->loadChangedConnection,
            impl_->buttonPressEventConnection,
            impl_->buttonReleaseEventConnection,
            impl_->titleChangedConnection
        }) {
            g_signal_handler_disconnect(impl_->gtkWebView, connection);
        }

        WebKitUserContentManager* manager = webkit_web_view_get_user_content_manager(impl_->gtkWebView);
        for (gulong connection: {
            impl_->scriptStringMessageConnection,
            impl_->scriptWindowDragConnection
        }) {
            g_signal_handler_disconnect(manager, connection);
        }

        g_object_unref(impl_->gtkWebView);
    }

    void WebView::LoadLocalFile(const std::string& path) {
        const char* cpath = path.c_str();
        gchar* folderPath = g_path_get_dirname(cpath);
        gchar* filename = g_path_get_basename(cpath);
        gchar* encodedFilename = g_uri_escape_string(filename, nullptr, false);
        gchar* url = g_strdup_printf("%s://host/%s", localURLScheme, encodedFilename);

        impl_->servedPath.emplace(folderPath);
        webkit_web_view_load_uri(impl_->gtkWebView, url);

        g_free(folderPath);
        g_free(filename);
        g_free(encodedFilename);
        g_free(url);
    }

    void WebView::LoadRequest(
        const std::string& method,
        const std::string& urlString,
        const std::vector<HTTPHeader>& headers,
        const std::optional<std::string>& body
    ) {
        impl_->servedPath.reset();

        WebKitURIRequest* request = webkit_uri_request_new(urlString.c_str());

        SoupMessageHeaders* requestHeaders = webkit_uri_request_get_http_headers(request);

        for (const HTTPHeader& header: headers) {
            soup_message_headers_append(requestHeaders, header.field.c_str(), header.value.c_str());
        }

        webkit_web_view_load_request(impl_->gtkWebView, request);
        g_object_unref(request);
    }

    void WebView::SetDevToolsEnabled(bool enabled) {
        WebKitSettings* settings = webkit_web_view_get_settings(impl_->gtkWebView);
        webkit_settings_set_enable_developer_extras(settings, enabled);
    }

    void WebView::Reload() {
        webkit_web_view_reload_bypass_cache(impl_->gtkWebView);
    }

    void WebView::ExecuteJavaScript(const std::string& scriptString, std::optional<JavaScriptExecutionCallback>&& optionalCallback) {
        if (!optionalCallback.has_value()) {
            webkit_web_view_run_javascript(impl_->gtkWebView, scriptString.c_str(), nullptr, nullptr, nullptr);
        }
        else {
            webkit_web_view_run_javascript(
                impl_->gtkWebView, scriptString.c_str(), nullptr,
                [](GObject* object, GAsyncResult* asyncResult, gpointer user_data) {
                    JavaScriptExecutionCallback* callbackPtr = static_cast<JavaScriptExecutionCallback*>(user_data);
                    JavaScriptExecutionCallback callback(std::move(*callbackPtr));
                    delete callbackPtr;

                    WebKitJavascriptResult *jsResult;
                    GError *error = NULL;

                    jsResult = webkit_web_view_run_javascript_finish(WEBKIT_WEB_VIEW(object), asyncResult, &error);
                    if (jsResult == nullptr) {
                        callback(std::make_optional<std::string>(error->message));
                        g_error_free (error);
                        return;
                    }

                    callback(std::nullopt);

                    webkit_javascript_result_unref(jsResult);
                },
                new JavaScriptExecutionCallback(std::move(*optionalCallback))
            );
        };
    }
}
