#include "../window/browser_window.h"
#include "menu_impl.h"
#include "webview_impl.h"
#include "./BrowserWindow_impl.h"

namespace DeskGap {


    bool BrowserWindow::Impl::handleDeleteEvent(GtkWidget*, GdkEvent*, BrowserWindow* window) {
        window->impl_->callbacks.onClose();
        return true;
    }

    
    BrowserWindow::BrowserWindow(const WebView& webView, EventCallbacks&& callbacks): impl_(std::make_unique<Impl>()) {
        impl_->callbacks = std::move(callbacks);

        GtkWindow* gtkWindow = GTK_WINDOW(g_object_ref_sink(gtk_window_new(GTK_WINDOW_TOPLEVEL)));

        gtk_container_add(GTK_CONTAINER(gtkWindow), GTK_WIDGET(webView.impl_->gtkWebView));

        impl_->deleteEventConnection = g_signal_connect(gtkWindow, "delete-event", G_CALLBACK(Impl::handleDeleteEvent), this);
        
        impl_->gtkWindow = gtkWindow;
    }

    BrowserWindow::~BrowserWindow() {
        printf("deinit browser_window\n");
        for (gulong connection: { impl_->deleteEventConnection }) {
            if (connection > 0) {
                g_signal_handler_disconnect(impl_->gtkWindow, connection);
            }
        }
        g_object_unref(impl_->gtkWindow);
    }

    void BrowserWindow::Show() {
        gtk_widget_show(GTK_WIDGET(impl_->gtkWindow));
    }

    void BrowserWindow::SetMaximizable(bool maximizable) {

    }
    void BrowserWindow::SetMinimizable(bool minimizable) {

    }
    void BrowserWindow::SetResizable(bool resizable) {
        gtk_window_set_resizable(impl_->gtkWindow, resizable);
    }
    void BrowserWindow::SetHasFrame(bool hasFrame) {
        gtk_window_set_decorated(impl_->gtkWindow, hasFrame);
    }

    void BrowserWindow::SetTitle(const std::string& utf8title) {
        gtk_window_set_title(impl_->gtkWindow, utf8title.c_str());
    }

    void BrowserWindow::SetClosable(bool closable) {
        gtk_window_set_deletable(impl_->gtkWindow, closable);
    }

    void BrowserWindow::SetSize(int width, int height, bool animate) {
        gtk_window_resize(impl_->gtkWindow, width, height);
    }

    void BrowserWindow::SetMaximumSize(int width, int height) {
        GdkGeometry geometry; 
        geometry.max_height = (height == 0 ? INT_MAX: height);
        geometry.max_width = (width == 0 ? INT_MAX: width);
        gtk_window_set_geometry_hints(impl_->gtkWindow, nullptr, &geometry, GDK_HINT_MAX_SIZE);
    }
    void BrowserWindow::SetMinimumSize(int width, int height) {
        GdkGeometry geometry; 
        geometry.min_height = height;
        geometry.min_width = width;
        gtk_window_set_geometry_hints(impl_->gtkWindow, nullptr, &geometry, GDK_HINT_MIN_SIZE); 
    }

    void BrowserWindow::SetPosition(int x, int y, bool animate) {
        gtk_window_move(impl_->gtkWindow, x, y);
    }

    std::array<int, 2> BrowserWindow::GetSize() {
        int width, height;
        gtk_window_get_size(impl_->gtkWindow, &width, &height);
        return { width, height };
    }

    std::array<int, 2> BrowserWindow::GetPosition() {
        int x, y;
        gtk_window_get_position(impl_->gtkWindow, &x, &y);
        return { x, y };
    }

    void BrowserWindow::Minimize() {
        gtk_window_iconify(impl_->gtkWindow);
    }

    void BrowserWindow::Center() {
        gtk_window_set_position(impl_->gtkWindow, GTK_WIN_POS_CENTER);
    }

    void BrowserWindow::SetMenu(const Menu* menu) {
        
    }

    void BrowserWindow::SetIcon(const std::optional<std::string>& iconPath) {
        if (iconPath.has_value()) {
            GError* error;
            gtk_window_set_icon_from_file(impl_->gtkWindow, iconPath->c_str(), &error);
            //throw...
        }
        else {
            gtk_window_set_icon(impl_->gtkWindow, nullptr);
        }
    }


    void BrowserWindow::Destroy() {
        g_signal_handler_disconnect(impl_->gtkWindow, impl_->deleteEventConnection);
        impl_->deleteEventConnection = 0;
        gtk_window_close(impl_->gtkWindow);
    }
    void BrowserWindow::Close() {
        gtk_window_close(impl_->gtkWindow);
    }

    void BrowserWindow::PopupMenu(const Menu& menu, const std::array<int, 2>* location, int positioningItem) {
        
    }

}
