#include "browser_window.hpp"
#include "menu_impl.h"
#include "webview_impl.h"
#include "./BrowserWindow_impl.h"
#include "./glib_exception.h"

namespace DeskGap {
    int i = 0;

    bool BrowserWindow::Impl::HandleDeleteEvent(GtkWidget*, GdkEvent*, BrowserWindow* window) {
        window->impl_->callbacks.onClose();
        return true;
    }

    bool BrowserWindow::Impl::HandleFocusInEvent(GtkWidget*, GdkEvent*, BrowserWindow* window) {
        window->impl_->callbacks.onFocus();
        return FALSE;
    }

    bool BrowserWindow::Impl::HandleFocusOutEvent(GtkWidget*, GdkEvent*, BrowserWindow* window) {
        window->impl_->callbacks.onBlur();
        return FALSE;
    }

    bool BrowserWindow::Impl::HandleConfigureEvent(GtkWidget*, GdkEventConfigure* eventConfigure, BrowserWindow* window) {
        std::optional<Rect>& lastRect = window->impl_->lastRect;
        const BrowserWindow::EventCallbacks& callbacks = window->impl_->callbacks;
        if (!lastRect.has_value()) {
            callbacks.onResize();
            callbacks.onMove();
        }
        else {
            if (eventConfigure->x != lastRect->x || eventConfigure->y != lastRect->y) {
                callbacks.onMove();
            }
            if (eventConfigure->width != lastRect->width || eventConfigure->height != lastRect->height) {
                callbacks.onResize();
            }
        }
        lastRect.emplace(Rect { eventConfigure->x, eventConfigure->y, eventConfigure->width, eventConfigure->height });
        return FALSE;
    }

    BrowserWindow::Impl::AccelGroupMenu::AccelGroupMenu(const Menu& menu): menuBar(GTK_WIDGET(menu.impl_->gtkMenuShell)) {
        accelGroup = gtk_accel_group_new();
        menu.impl_->SetAccelGroup(accelGroup);
    }
    BrowserWindow::Impl::AccelGroupMenu::~AccelGroupMenu() {
        g_object_unref(accelGroup);
    }

    
    BrowserWindow::BrowserWindow(const WebView& webView, EventCallbacks&& callbacks): impl_(std::make_unique<Impl>()) {
        impl_->callbacks = std::move(callbacks);
        impl_->webViewWidget = GTK_WIDGET(webView.impl_->gtkWebView);

        GtkWindow* gtkWindow = GTK_WINDOW(g_object_ref_sink(gtk_window_new(GTK_WINDOW_TOPLEVEL)));

        GtkBox* gtkBox = GTK_BOX(g_object_ref_sink(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0)));
        gtk_widget_show(GTK_WIDGET(gtkBox));

        gtk_box_pack_end(gtkBox, impl_->webViewWidget, true, true, 0);
        gtk_container_add(GTK_CONTAINER(gtkWindow), GTK_WIDGET(gtkBox));

        impl_->deleteEventConnection = g_signal_connect(gtkWindow, "delete-event", G_CALLBACK(Impl::HandleDeleteEvent), this);
        impl_->focusInEventConnection = g_signal_connect(gtkWindow, "focus-in-event", G_CALLBACK(Impl::HandleFocusInEvent), this);
        impl_->focusOutEventConnection = g_signal_connect(gtkWindow, "focus-out-event", G_CALLBACK(Impl::HandleFocusOutEvent), this);
        impl_->configureEventConnection = g_signal_connect(gtkWindow, "configure-event", G_CALLBACK(Impl::HandleConfigureEvent), this);

        impl_->gtkWindow = gtkWindow;
        impl_->gtkBox = gtkBox;
    }

    BrowserWindow::~BrowserWindow() {
        g_object_unref(impl_->gtkBox);
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
        GdkDisplay* display = gtk_widget_get_display(GTK_WIDGET(impl_->gtkWindow));
        GdkScreen* screen = gdk_display_get_default_screen(display);

        gint screenWidth = gdk_screen_get_width(screen);
        gint screenHeight = gdk_screen_get_height(screen);

        int windowWidth, windowHeight;
        gtk_window_get_size(impl_->gtkWindow, &windowWidth, &windowHeight);

        gtk_window_move(impl_->gtkWindow, (screenWidth - windowWidth) / 2, (screenHeight - windowHeight) / 2);
    }

    void BrowserWindow::SetMenu(const Menu* menu) {
        if (impl_->accelGroupMenu.has_value()) {
            gtk_container_remove(GTK_CONTAINER(impl_->gtkBox), impl_->accelGroupMenu->menuBar);
            gtk_window_remove_accel_group(impl_->gtkWindow, impl_->accelGroupMenu->accelGroup);
            impl_->accelGroupMenu.reset();
        }

        if (menu != nullptr) {
            impl_->accelGroupMenu.emplace(*menu);
            gtk_window_add_accel_group(impl_->gtkWindow, impl_->accelGroupMenu->accelGroup);
            gtk_box_pack_start(impl_->gtkBox, impl_->accelGroupMenu->menuBar, FALSE, FALSE, 0);
            //gtk_box_reorder_child(impl_->gtkBox, impl_->accelGroupMenu->menuBar, 0);
        }

    }

    void BrowserWindow::SetIcon(const std::optional<std::string>& iconPath) {
        if (iconPath.has_value()) {
            GError* error;
            gtk_window_set_icon_from_file(impl_->gtkWindow, iconPath->c_str(), &error);
            GlibException::ThrowAndFree(error);
        }
        else {
            gtk_window_set_icon(impl_->gtkWindow, nullptr);
        }
    }

    void BrowserWindow::Destroy() {
        for (gulong connection: { 
            impl_->deleteEventConnection,
            impl_->focusInEventConnection,
            impl_->focusOutEventConnection,
            impl_->configureEventConnection
        }) {
            g_signal_handler_disconnect(impl_->gtkWindow, connection);
        }
        gtk_widget_destroy(GTK_WIDGET(impl_->gtkWindow));
    }
    void BrowserWindow::Close() {
        gtk_window_close(impl_->gtkWindow);
    }

    namespace {
        struct PopupMenuDeactivateSignalData {
            std::function<void()> onClose;
            gulong connection;
        };
        void HandlePopupMenuDeactivate(GtkMenuShell* menuShell, PopupMenuDeactivateSignalData* data) {
            g_signal_handler_disconnect(menuShell, data->connection);
            data->onClose();
            delete data;
        }
    }

    void BrowserWindow::PopupMenu(const Menu& menu, const std::array<int, 2>* location, int positioningItem, std::function<void()>&& onClose) {
        GtkMenuPositionFunc positionFunc = nullptr;
        gpointer positionFuncData = nullptr;

        if (location != nullptr) {
            auto rootLocation = new std::array<int, 2>();
            GdkWindow* webViewGDKWindow = gtk_widget_get_window(impl_->webViewWidget);
            gdk_window_get_root_coords(
                webViewGDKWindow,
                std::get<0>(*location), std::get<1>(*location),
                &((*rootLocation)[0]), &((*rootLocation)[1])
            );

            positionFuncData = rootLocation;
            positionFunc = [](GtkMenu*, gint *x, gint *y, gboolean* push_in, gpointer user_data) {
                auto rootLocation = static_cast<std::array<int, 2>*>(user_data);
                *x = std::get<0>(*rootLocation);
                *y = std::get<1>(*rootLocation);
                delete rootLocation;
            };
        }
        auto signalData = new PopupMenuDeactivateSignalData { std::move(onClose), 0 };
        signalData->connection = g_signal_connect(menu.impl_->gtkMenuShell, "deactivate", G_CALLBACK(HandlePopupMenuDeactivate), signalData);
        gtk_menu_popup(
            GTK_MENU(menu.impl_->gtkMenuShell), nullptr, nullptr, 
            positionFunc, positionFuncData,
            0, GDK_CURRENT_TIME
        );
    }
}
