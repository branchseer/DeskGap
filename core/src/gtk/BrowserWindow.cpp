#include "../window/browser_window.h"
#include "menu_impl.h"
#include "webview_impl.h"
#include "./BrowserWindow_impl.h"

namespace DeskGap {
    
    BrowserWindow::BrowserWindow(const WebView& webView, EventCallbacks&& callbacks): impl_(std::make_unique<Impl>()) {
        impl_->gtkWindow = std::make_unique<Gtk::Window>();
        impl_->deleteConnection = impl_->gtkWindow->signal_delete_event().connect([onClose = std::move(callbacks.onClose)](GdkEventAny*) {
            onClose();
            return true;
        });
    }

    void BrowserWindow::Show() {
        impl_->gtkWindow->show();
    }

    void BrowserWindow::SetMaximizable(bool maximizable) {

    }
    void BrowserWindow::SetMinimizable(bool minimizable) {

    }
    void BrowserWindow::SetResizable(bool resizable) {
        impl_->gtkWindow->set_resizable(resizable);
    }
    void BrowserWindow::SetHasFrame(bool hasFrame) {
        impl_->gtkWindow->set_decorated(hasFrame);
    }

    void BrowserWindow::SetTitle(const std::string& utf8title) {
        impl_->gtkWindow->set_title(utf8title);
    }

    void BrowserWindow::SetClosable(bool closable) {
        impl_->gtkWindow->set_deletable(closable);
    }

    void BrowserWindow::SetSize(int width, int height, bool animate) {
        impl_->gtkWindow->resize(width, height);
    }

    void BrowserWindow::SetMaximumSize(int width, int height) {
        Gdk::Geometry geometry; 
        geometry.max_height = (height == 0 ? INT_MAX: height);
        geometry.max_width = (width == 0 ? INT_MAX: width);
        impl_->gtkWindow->set_geometry_hints(*(impl_->gtkWindow), geometry, Gdk::HINT_MAX_SIZE); 
    }
    void BrowserWindow::SetMinimumSize(int width, int height) {
        Gdk::Geometry geometry; 
        geometry.min_height = height;
        geometry.min_width = width;
        impl_->gtkWindow->set_geometry_hints(*(impl_->gtkWindow), geometry, Gdk::HINT_MIN_SIZE); 
    }

    void BrowserWindow::SetPosition(int x, int y, bool animate) {
        impl_->gtkWindow->move(x, y);
    }

    std::array<int, 2> BrowserWindow::GetSize() {
        int width, height;
        impl_->gtkWindow->get_size(width, height);
        return { width, height };
    }

    std::array<int, 2> BrowserWindow::GetPosition() {
        int x, y;
        impl_->gtkWindow->get_position(x, y);
        return { x, y };
    }

    void BrowserWindow::Minimize() {
        impl_->gtkWindow->iconify();
    }

    void BrowserWindow::Center() {
        impl_->gtkWindow->set_position(Gtk::WIN_POS_CENTER);
    }

    void BrowserWindow::SetMenu(const Menu* menu) {

    }

    void BrowserWindow::SetIcon(const std::optional<std::string>& iconPath) {
        if (iconPath.has_value()) {
            impl_->gtkWindow->set_icon_from_file(*iconPath);
        }
        else {
            impl_->gtkWindow->set_icon({ });
        }
    }


    void BrowserWindow::Destroy() {
        impl_->deleteConnection.disconnect();
        impl_->gtkWindow->close();
    }
    void BrowserWindow::Close() {
        impl_->gtkWindow->close();
    }

    void BrowserWindow::PopupMenu(const Menu& menu, const std::array<int, 2>* location, int positioningItem) {
        
    }

    BrowserWindow::~BrowserWindow() = default;
}
