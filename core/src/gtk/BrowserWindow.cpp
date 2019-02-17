#include "../window/browser_window.h"
#include "menu_impl.h"
#include "webview_impl.h"
#include "./BrowserWindow_impl.h"

namespace DeskGap {
    
    BrowserWindow::BrowserWindow(const WebView& webView, const EventCallbacks& callbacks): impl_(std::make_unique<Impl>()) {
        impl_->gtkWindow = std::make_unique<Gtk::Window>();
    }

    void BrowserWindow::Show() {
        impl_->gtkWindow->show();
    }

    void BrowserWindow::SetMaximizable(bool maximizable) {

    }
    void BrowserWindow::SetMinimizable(bool minimizable) {

    }
    void BrowserWindow::SetResizable(bool resizable) {

    }
    void BrowserWindow::SetHasFrame(bool hasFrame) {

    }

    void BrowserWindow::SetTitle(const std::string& utf8title) {

    }

    void BrowserWindow::SetClosable(bool closable) {

    }

    void BrowserWindow::SetSize(int width, int height, bool animate) {
        impl_->gtkWindow->resize(width, height);
    }

    void BrowserWindow::SetMaximumSize(int width, int height) {

    }
    void BrowserWindow::SetMinimumSize(int width, int height) {

    }

    void BrowserWindow::SetPosition(int x, int y, bool animate) {
        impl_->gtkWindow->move(x, y);
    }

    std::array<int, 2> BrowserWindow::GetSize() {
        return {0,0};
    }

    std::array<int, 2> BrowserWindow::GetPosition() {
        return {0,0};
    }

    void BrowserWindow::Minimize() {

    }

    void BrowserWindow::Center() {
        impl_->gtkWindow->set_position(Gtk::WIN_POS_CENTER);
    }

    void BrowserWindow::SetMenu(const Menu* menu) {

    }

    void BrowserWindow::SetIcon(const std::optional<std::string>& iconPath) {

    }


    void BrowserWindow::Destroy() {

    }
    void BrowserWindow::Close() {

    }

    void BrowserWindow::PopupMenu(const Menu& menu, const std::array<int, 2>* location, int positioningItem) {
        
    }

    BrowserWindow::~BrowserWindow() = default;
}
