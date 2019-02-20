#include <filesystem>

#include <gtk/gtk.h>
#include <fstream>

#include "../webview/webview.h"
#include "webview_impl.h"
#include "../lib_path.h"
#include "../util/mime.h"
#include "../lib_path.h"
#include "./glib_exception.h"


namespace fs = std::filesystem;

namespace {
    const gchar* localURLScheme = "deskgap-local";
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

                if (const char* firstDot = strrchr(filename, '.'); firstDot != nullptr) {
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
        case WEBKIT_LOAD_STARTED:
            webView->impl_->callbacks.didStartLoading();
            break;
        case WEBKIT_LOAD_FINISHED:
            webView->impl_->callbacks.didStopLoading(std::nullopt);
            break;
        default:
            break;
        }
    }

    WebView::WebView(EventCallbacks&& callbacks): impl_(std::make_unique<Impl>()) {
        impl_->callbacks = std::move(callbacks);
        
        WebKitWebContext* context = webkit_web_context_new();
        webkit_web_context_register_uri_scheme(
            context,
            localURLScheme, Impl::HandleLocalFileUriSchemeRequest,
            this, nullptr
        );

        impl_->gtkWebView = WEBKIT_WEB_VIEW(g_object_ref_sink(webkit_web_view_new_with_context(context)));
        g_object_unref(context);

        WebKitUserContentManager* manager = webkit_web_view_get_user_content_manager(impl_->gtkWebView);

        static gchar* preloadScript = nullptr;
        if (preloadScript == nullptr) {
            fs::path scriptFolder = fs::path(LibPath()) / "dist" / "ui";

            for (const char* scriptFilename: { "preload_gtk.js", "preload.js" }) {
                fs::path scriptPath = scriptFolder / scriptFilename;

                gchar* fileContent;
                gsize fileSize;
                GError* error = nullptr;
                g_file_get_contents(scriptPath.c_str(), &fileContent, &fileSize, &error);
                GlibException::ThrowAndFree(error);

                if (preloadScript == nullptr) {
                    preloadScript = fileContent;
                }
                else {
                    gchar* oldPreloadScript = preloadScript;
                    preloadScript = g_strconcat(preloadScript, fileContent, nullptr);
                    g_free(oldPreloadScript);
                    g_free(fileContent);
                }
            }
        }



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

        WebKitUserScript* preloadUserScript = webkit_user_script_new(
            preloadScript, 
            WEBKIT_USER_CONTENT_INJECT_TOP_FRAME,
            WEBKIT_USER_SCRIPT_INJECT_AT_DOCUMENT_START,
            nullptr, nullptr
        );
        webkit_user_content_manager_add_script(manager, preloadUserScript);
        webkit_user_script_unref(preloadUserScript);

        gtk_widget_show(GTK_WIDGET(impl_->gtkWebView));
        impl_->loadChangedConnection = g_signal_connect(impl_->gtkWebView, "load-changed", G_CALLBACK(Impl::HandleLoadChanged), this);
        impl_->buttonPressEventConnection = g_signal_connect(impl_->gtkWebView, "button-press-event", G_CALLBACK(Impl::HandleButtonPressEvent), this);
        impl_->buttonReleaseEventConnection = g_signal_connect(impl_->gtkWebView, "button-release-event", G_CALLBACK(Impl::HandleButtonReleaseEvent), this);
    }

    void WebView::Impl::HandleScriptWindowDrag(WebKitUserContentManager*, WebKitJavascriptResult*, WebView* webView) {
        printf("drag?\n");
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
        JSCValue* jsValue = webkit_javascript_result_get_js_value(jsResult);
        GBytes* stringMessageGBytes = jsc_value_to_string_as_bytes(jsValue);

        gsize size;
        gconstpointer stringMessageBytes = g_bytes_get_data(stringMessageGBytes, &size);
        std::string stringMessage(static_cast<const char*>(stringMessageBytes), size);

        g_bytes_unref(stringMessageGBytes);

        webView->impl_->callbacks.onStringMessage(std::move(stringMessage));
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
            impl_->buttonReleaseEventConnection
        }) {
            if (connection > 0) {
                g_signal_handler_disconnect(impl_->gtkWebView, connection);
            }
        }

        WebKitUserContentManager* manager = webkit_web_view_get_user_content_manager(impl_->gtkWebView);
        for (gulong connection: {
            impl_->scriptStringMessageConnection,
            impl_->scriptWindowDragConnection
        }) {
            if (connection > 0) {
                g_signal_handler_disconnect(manager, connection);
            }
        }

        g_object_unref(impl_->gtkWebView);
    }

    void WebView::LoadHTMLString(const std::string& html) {
        impl_->servedPath.reset();
        webkit_web_view_load_html(impl_->gtkWebView, html.c_str(), nullptr);
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
        webkit_settings_set_enable_developer_extras(settings, true);
    }

    void WebView::Reload() {
        webkit_web_view_reload_bypass_cache(impl_->gtkWebView);
    }

    void WebView::EvaluateJavaScript(const std::string& scriptString, std::optional<JavaScriptEvaluationCallback>&& optionalCallback) {
        if (!optionalCallback.has_value()) {
            webkit_web_view_run_javascript(impl_->gtkWebView, scriptString.c_str(), nullptr, nullptr, nullptr);
        }
        else {
            webkit_web_view_run_javascript(
                impl_->gtkWebView, scriptString.c_str(), nullptr,
                [](GObject* object, GAsyncResult* asyncResult, gpointer user_data) {
                    JavaScriptEvaluationCallback* callbackPtr = static_cast<JavaScriptEvaluationCallback*>(user_data);
                    JavaScriptEvaluationCallback callback(std::move(*callbackPtr));
                    delete callbackPtr;

                    WebKitJavascriptResult *jsResult;
                    JSCValue *jsResultValue;
                    GError *error = NULL;

                    jsResult = webkit_web_view_run_javascript_finish(WEBKIT_WEB_VIEW(object), asyncResult, &error);
                    if (jsResult == nullptr) {
                        callback(true, error->message);
                        g_error_free (error);
                        return;
                    }
                    jsResultValue = webkit_javascript_result_get_js_value(jsResult);
                    if (jsc_value_is_string(jsResultValue)) {
                        JSCException* jsException;
                        gchar* stringResult;

                        stringResult = jsc_value_to_string(jsResultValue);
                        jsException = jsc_context_get_exception(jsc_value_get_context(jsResultValue));
                        if (jsException != nullptr) {
                            callback(true, jsc_exception_get_message(jsException));
                        }
                        else {
                            callback(true, stringResult);
                        }
                        g_free(stringResult);
                    } else {
                        callback(true, std::string());
                    }
                    webkit_javascript_result_unref(jsResult);
                },
                new JavaScriptEvaluationCallback(std::move(*optionalCallback))
            );
        };
    }
}
