#include <filesystem>

#include <gtk/gtk.h>
#include <fstream>

#include "../webview/webview.h"
#include "webview_impl.h"
#include "../lib_path.h"
#include "../util/mime.h"

namespace fs = std::filesystem;

namespace {
    void webview_load_changed(GtkWidget*, WebKitLoadEvent load_event, DeskGap::WebView::EventCallbacks* callbacks) {
        switch (load_event) {
        case WEBKIT_LOAD_STARTED:
            callbacks->didStartLoading();
            break;
        case WEBKIT_LOAD_FINISHED:
            callbacks->didStopLoading(std::nullopt);
            break;
        default:
            break;
        }
    }
    const gchar* localURLScheme = "deskgap-local";

    void local_file_uri_scheme_request_cb(WebKitURISchemeRequest *request, gpointer servedPathPtr) {
        const auto& servedPath = *static_cast<std::optional<std::string>*>(servedPathPtr);
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
}

namespace DeskGap {

    WebView::WebView(EventCallbacks&& callbacks): impl_(std::make_unique<Impl>()) {
        impl_->callbacks = std::move(callbacks);
        
        WebKitWebContext* context = webkit_web_context_new();
        webkit_web_context_register_uri_scheme(
            context,
            localURLScheme, local_file_uri_scheme_request_cb,
            &(impl_->servedPath), nullptr
        );

        impl_->gtkWebView = WEBKIT_WEB_VIEW(g_object_ref_sink(webkit_web_view_new_with_context(context)));
        g_object_unref(context);

        WebKitUserContentManager* contentManager = webkit_web_view_get_user_content_manager(impl_->gtkWebView);

        static gchar* preloadScript = nullptr;
        if (preloadScript == nullptr) {
            
        }

        gtk_widget_show(GTK_WIDGET(impl_->gtkWebView));
        impl_->loadChangedHandler = g_signal_connect(impl_->gtkWebView, "load-changed", G_CALLBACK(webview_load_changed), &(impl_->callbacks));
    }


    WebView::~WebView() {
        printf("deinit webview\n");

        for (gulong handler: { impl_->loadChangedHandler }) {
            g_signal_handler_disconnect(impl_->gtkWebView, handler);
        }
        g_object_unref(impl_->gtkWebView);
    }

    void WebView::LoadHTMLString(const std::string& html) {
        impl_->servedPath.reset();
        webkit_web_view_load_html(impl_->gtkWebView, html.c_str(), nullptr);
    }

    void WebView::LoadLocalFile(const std::string& path) {
        printf("%s\n", "LoadLocalFile");
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
        //printf("%s\n", scriptString.c_str());
    }
}
