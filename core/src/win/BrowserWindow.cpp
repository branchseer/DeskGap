#include <Windows.h>
#include <vcclr.h>

#include "../window/browser_window.h"
#include "./util/string_convert.h"
#include "./util/control_geometry.h"
#include "menu_impl.h"
#include "webview_impl.h"
#include "./BrowserWindow_impl.h"

using namespace System;

using System::Windows::Forms::Form;
using System::Windows::Forms::AutoScaleMode;
using System::Windows::Forms::Screen;
using System::Windows::Forms::Cursor;
using System::Windows::Forms::Control;
using System::Windows::Forms::FormClosingEventArgs;
using System::Windows::Forms::FormClosingEventHandler;
using System::Windows::Forms::FormClosedEventArgs;
using System::Windows::Forms::FormClosedEventHandler;
using System::Windows::Forms::FormBorderStyle;
using System::Windows::Forms::FormWindowState;
using System::Windows::Forms::CreateParams;
using WinformsMenu = System::Windows::Forms::Menu;
using WinformsMainMenu = System::Windows::Forms::MainMenu;
using WinformsMenuItem = System::Windows::Forms::MenuItem;
using WinformsContextMenu = System::Windows::Forms::ContextMenu;
using namespace System::Drawing;

using System::Windows::Forms::Label;
using UWPWebView = Microsoft::Toolkit::Forms::UI::Controls::WebView;

namespace DeskGap {
    namespace {
        ref class DeskGapForm: public Form {
        private:
            bool closeBox_ = true;
        public:
        //The closeBox_ is used during the Form creation (CreateParams),
        //so it's only valid for the CloseBox to be set before the form is shown.
        //We make sure of this by exposing closable only in the constructor, not as a setter method in js land.
            property bool CloseBox {
                bool get() {
                    return closeBox_;
                }
                void set(bool value) {
                    closeBox_ = value;
                }
            }
        protected:
            property ::CreateParams^ CreateParams {
                virtual ::CreateParams^ get() override {
                    ::CreateParams^ cp = Form::CreateParams;
                    if (!closeBox_) {
                        cp->ClassStyle |= CS_NOCLOSE;
                    }
                    return cp;
                }
            }
        };

        ref class WindowCallbackHandler {
        private:
            const BrowserWindow::EventCallbacks* const callbacks_;
            bool ignoresOnClose_;
        public:
            ~WindowCallbackHandler() {
                delete callbacks_;
            }
            WindowCallbackHandler(const BrowserWindow::EventCallbacks* callbacks):
                callbacks_(new BrowserWindow::EventCallbacks(*callbacks)), ignoresOnClose_(false) {
                
            }

            void IgnoreOnClose() {
                ignoresOnClose_ = true;
            }

            void OnClosing(Object^, FormClosingEventArgs^ e) {
                if (!ignoresOnClose_) {
                    callbacks_->onClose();
                    e->Cancel = true;
                }
            }
            // void OnClosed(Object^, FormClosedEventArgs^) { callbacks_->onClosed(); }
            void OnActivated(Object^, EventArgs^) { callbacks_->onFocus(); }
            void OnDeactivate(Object^, EventArgs^) { callbacks_->onBlur(); }
            void OnResize(Object^, EventArgs^) { callbacks_->onResize(); }
            void OnMove(Object^, EventArgs^) { callbacks_->onMove(); }
        };
    }

    void BrowserWindow::Impl::updateFrame() {
        if (resizable && hasFrame) {
            form->FormBorderStyle = FormBorderStyle::Sizable;
            form->ControlBox = true;
            form->Text = formTitle;
        }
        else if (resizable && !hasFrame) {
            form->FormBorderStyle = FormBorderStyle::Sizable;
            form->ControlBox = false;
            form->Text = String::Empty;
        }
        else if (!resizable && hasFrame) {
            form->FormBorderStyle = FormBorderStyle::FixedSingle;
            form->ControlBox = true;
            form->Text = formTitle;
        }
        else if (!resizable && !hasFrame) {
            form->FormBorderStyle = FormBorderStyle::None;
        }
    };
    BrowserWindow::BrowserWindow(const WebView& webView, const EventCallbacks& callbacks): impl_(new Impl) {
        DeskGapForm^ form = gcnew DeskGapForm();

        form->AutoScaleMode = AutoScaleMode::Font;

        WindowCallbackHandler^ handler = gcnew WindowCallbackHandler(&callbacks);
        form->FormClosing += gcnew FormClosingEventHandler(handler, &WindowCallbackHandler::OnClosing);

        form->Activated += gcnew EventHandler(handler, &WindowCallbackHandler::OnActivated);
        form->Deactivate += gcnew EventHandler(handler, &WindowCallbackHandler::OnDeactivate);
        form->Resize += gcnew EventHandler(handler, &WindowCallbackHandler::OnResize);
        form->Move += gcnew EventHandler(handler, &WindowCallbackHandler::OnMove);

        UWPWebView^ uwpWebView = webView.impl_->uwpWebView;
        form->Controls->Add(uwpWebView);

        impl_->form = form;
        impl_->callbackHandler = handler;
    }

    void BrowserWindow::Show() {
        impl_->form->Show();
        HWND handle = static_cast<HWND>(impl_->form->Handle.ToPointer());
    }

    void BrowserWindow::SetMaximizable(bool maximizable) {
        impl_->form->MaximizeBox = maximizable;
    }
    void BrowserWindow::SetMinimizable(bool minimizable) {
        impl_->form->MinimizeBox = minimizable;
    }
    void BrowserWindow::SetResizable(bool resizable) {
        impl_->resizable = resizable;
        impl_->updateFrame();
    }
    void BrowserWindow::SetHasFrame(bool hasFrame) {
        impl_->hasFrame = hasFrame;
        impl_->updateFrame();
    }

    void BrowserWindow::SetTitle(const std::string& utf8title) {
        impl_->formTitle = ClrStr(utf8title);
        impl_->updateFrame();
    }

    void BrowserWindow::SetClosable(bool closable) {
        Form^ form = impl_->form;
        safe_cast<DeskGapForm^>(form)->CloseBox = closable;
    }

    void BrowserWindow::SetSize(int width, int height, bool animate) {
        Form^ form = impl_->form;
        std::array<int, 2> deviceSize = LogicalToDevice(form, { width, height });
        form->Size = Size(deviceSize[0], deviceSize[1]);
    }

    void BrowserWindow::SetMaximumSize(int width, int height) {
        Form^ form = impl_->form;
        std::array<int, 2> deviceSize = LogicalToDevice(form, { width, height });
        form->MaximumSize = Size(deviceSize[0], deviceSize[1]);
    }
    void BrowserWindow::SetMinimumSize(int width, int height) {
        Form^ form = impl_->form;
        std::array<int, 2> deviceSize = LogicalToDevice(form, { width, height });
        form->MinimumSize = Size(deviceSize[0], deviceSize[1]);
    }

    void BrowserWindow::SetPosition(int x, int y, bool animate) {
        Form^ form = impl_->form;
        std::array<int, 2> devicePosition = LogicalToDevice(form, { x, y });
        form->DesktopLocation = Point(devicePosition[0], devicePosition[1]);
    }

    std::array<int, 2> BrowserWindow::GetSize() {
        Form^ form = impl_->form;
        return DeviceToLogical(form, {
            form->Size.Width,
            form->Size.Height
        });
    }

    std::array<int, 2> BrowserWindow::GetPosition() {
        Form^ form = impl_->form;
        return DeviceToLogical(form, {
            form->DesktopLocation.X,
            form->DesktopLocation.Y
        });
    }

    void BrowserWindow::Minimize() {
        impl_->form->WindowState = FormWindowState::Minimized;
    }

    void BrowserWindow::Center() {
        Form^ form = impl_->form;
        Screen^ screen = Screen::FromControl(form);
        form->DesktopLocation = Point(
            (screen->WorkingArea.Width - form->Width) / 2,
            (screen->WorkingArea.Height - form->Height) / 2
        );
    }

    void BrowserWindow::SetMenu(const Menu* menu) {
        Form^ form = impl_->form;

        if (menu == nullptr) {
            form->Menu = nullptr;
        }
        else {
            WinformsMenu^ winformsMenu = menu->impl_->winforms_menu;
            form->Menu = safe_cast<WinformsMainMenu^>(winformsMenu);
        }
    }

    void BrowserWindow::SetIcon(const std::optional<std::string>& iconPath) {
        using System::Drawing::Bitmap;
        using System::Drawing::Icon;

        if (!iconPath.has_value()) {
            impl_->form->ShowIcon = false;
            return;
        }

        String^ thePath = ClrStr(*iconPath);

        Icon^ icon;

        if (thePath->EndsWith(".ico")) {
            icon = gcnew Icon(thePath, Size(256, 256));
        }
        else {
            Bitmap^ iconBitmap = gcnew Bitmap(thePath);

            IntPtr hicon = iconBitmap->GetHicon();
            icon = Icon::FromHandle(hicon);
        }
        impl_->form->Icon = icon;
        impl_->form->ShowIcon = true;
    }


    void BrowserWindow::Destroy() {
        Object^ callbackHandler = impl_->callbackHandler;
        safe_cast<WindowCallbackHandler^>(callbackHandler)->IgnoreOnClose();
        impl_->form->Close();
    }
    void BrowserWindow::Close() {
        impl_->form->Close();
    }

    void BrowserWindow::PopupMenu(const Menu& menu, const std::array<int, 2>* location, int positioningItem) {
        Form^ form = impl_->form;
        WinformsMenu^ winformsMenu = menu.impl_->winforms_menu;
        WinformsContextMenu^ contextMenu = safe_cast<WinformsContextMenu^>(winformsMenu);

        Control^ control = form->Controls[0];

        Point popupLocation;
        if (location != nullptr) {
            popupLocation = Point((*location)[0], (*location)[1]);
        }
        else {
            popupLocation = control->PointToClient(Cursor::Position);
        }

        contextMenu->Show(control, popupLocation);
    }

    BrowserWindow::~BrowserWindow() = default;
}
