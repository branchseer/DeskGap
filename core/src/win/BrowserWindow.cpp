#include <Windows.h>

#include "menu_impl.h"
#include "webview_impl.h"
#include "./BrowserWindow_impl.h"
#include "./util/wstring_utf8.h"

namespace {
	const wchar_t* const BrowserWindowWndClassName = L"DeskGapBrowserWindow";
}

namespace DeskGap {
    BrowserWindow::BrowserWindow(const WebView& webView, EventCallbacks&& callbacks): impl_(std::make_unique<Impl>()) {
        static bool isClassRegistered = false;
        if (!isClassRegistered) {
            isClassRegistered = true;
            WNDCLASSEXW wndClass { };
            wndClass.cbSize = sizeof(WNDCLASSEXW);
            wndClass.hInstance = GetModuleHandleW(nullptr);
            wndClass.lpszClassName = BrowserWindowWndClassName;
            wndClass.lpfnWndProc = [](HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) -> LRESULT {
                BrowserWindow* browserWindow = reinterpret_cast<BrowserWindow*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
                if (browserWindow != nullptr) {
                    switch (msg)
                    {
                        case WM_CLOSE: {
                            browserWindow->impl_->callbacks.onClose();
                            return 0;
                        }
                        case WM_SIZE: {
                            RECT rect { };
                            GetClientRect(hwnd, &rect);
                            LONG width = rect.right - rect.left;
                            LONG height = rect.bottom - rect.top;
                            SetWindowPos(
                                browserWindow->impl_->webViewControlWnd, nullptr,
                                0, 0, width, height,
                                SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER
                            );
                            browserWindow->impl_->callbacks.onResize();
                            break;
                        }
                        case WM_MOVE: {
                            browserWindow->impl_->callbacks.onMove();
                            break;
                        }
                        case WM_DPICHANGED: {
                            RECT* rect = reinterpret_cast<RECT*>(lp);
                            SetWindowPos(
                                browserWindow->impl_->windowWnd, nullptr,
                                rect->left, rect->top,
                                rect->right - rect->left, rect->bottom - rect->top,
                                SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER
                            );
                            break;
                        }
                    }
                }
                return DefWindowProcW(hwnd, msg, wp, lp);
            };
            wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
            wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
            RegisterClassExW(&wndClass);
        }

        impl_->callbacks = std::move(callbacks);
        impl_->windowWnd = CreateWindowW(
            BrowserWindowWndClassName,
            L"", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
            CW_USEDEFAULT, 0, 0,
            nullptr, nullptr,
            GetModuleHandleW(nullptr),
            nullptr
        );
        SetWindowLongPtrW(impl_->windowWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
        
        impl_->webViewControlWnd = webView.impl_->InitControl(impl_->windowWnd);
    }

    void BrowserWindow::Show() {
        ShowWindow(impl_->windowWnd, SW_SHOW);
        UpdateWindow(impl_->windowWnd);

        ShowWindow(impl_->webViewControlWnd, SW_SHOW);
        UpdateWindow(impl_->webViewControlWnd);

        SetFocus(impl_->windowWnd);
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
        std::wstring wtitle = UTF8ToWString(utf8title.c_str());
        SetWindowTextW(impl_->windowWnd, wtitle.c_str());
    }

    void BrowserWindow::SetClosable(bool closable) {
        
    }

    void BrowserWindow::SetSize(int width, int height, bool animate) {
        int dpi = GetDpiForWindow(impl_->windowWnd); 
        int dpiScaledWidth = MulDiv(width, dpi, 96); 
        int dpiScaledHeight = MulDiv(height, dpi, 96); 
        SetWindowPos(
            impl_->windowWnd, nullptr, 0, 0,
            dpiScaledWidth, dpiScaledHeight,
            SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOMOVE
        );
    }

    void BrowserWindow::SetMaximumSize(int width, int height) {
        
    }
    void BrowserWindow::SetMinimumSize(int width, int height) {
        
    }

    void BrowserWindow::SetPosition(int x, int y, bool animate) {
        UINT dpi = GetDpiForWindow(impl_->windowWnd);
        LONG dpiScaledX = MulDiv(x, dpi, 96);
        LONG dpiScaledY = MulDiv(y, dpi, 96);
        SetWindowPos(
            impl_->windowWnd, nullptr,
            dpiScaledX, dpiScaledY, 0, 0,
            SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOSIZE
        );
    }

    std::array<int, 2> BrowserWindow::GetSize() {
        return { 0, 0 };
    }

    std::array<int, 2> BrowserWindow::GetPosition() {
        return { 0, 0 };
    }

    void BrowserWindow::Minimize() {

    }

    void BrowserWindow::Center() {
        RECT workAreaRect { };
        SystemParametersInfoW(SPI_GETWORKAREA, 0, &workAreaRect, 0);

        int desktopWidth = workAreaRect.right -workAreaRect.left;
        int desktopHeight = workAreaRect.bottom - workAreaRect.top;

        RECT windowRect { };
        GetClientRect(impl_->windowWnd, &windowRect);

        int windowWidth = windowRect.right - windowRect.left;
        int windowHeight = windowRect.bottom - windowRect.top;

        SetWindowPos(
            impl_->windowWnd, nullptr,
            workAreaRect.left + (desktopWidth - windowWidth) / 2,
            workAreaRect.top + (desktopHeight - windowHeight) / 2, 0, 0,
            SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOSIZE
        );
    }

    void BrowserWindow::SetMenu(const Menu* menu) {
        if (HMENU existingMenu = GetMenu(impl_->windowWnd); existingMenu != nullptr) {
            DestroyMenu(existingMenu);
        }
        ::SetMenu(impl_->windowWnd, menu == nullptr ? nullptr : menu->impl_->hmenu);
        if (menu != nullptr) {
            menu->impl_->SetWindowWnd(impl_->windowWnd);
        }
        DrawMenuBar(impl_->windowWnd);
    }

    void BrowserWindow::SetIcon(const std::optional<std::string>& iconPath) {
        
    }


    void BrowserWindow::Destroy() {
        SetWindowLongPtrW(impl_->windowWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(nullptr));
        DestroyWindow(impl_->windowWnd);
    }
    void BrowserWindow::Close() {

    }

    void BrowserWindow::PopupMenu(const Menu& menu, const std::array<int, 2>* location, int positioningItem) {
        SetForegroundWindow(impl_->windowWnd);

        UINT uFlags = TPM_RIGHTBUTTON;
        if (GetSystemMetrics(SM_MENUDROPALIGNMENT) != 0)
        {
            uFlags |= TPM_RIGHTALIGN;
        }
        else
        {
            uFlags |= TPM_LEFTALIGN;
        }

        POINT pt;
        if (location != nullptr) {
            pt.x = std::get<0>(*location);
            pt.y = std::get<1>(*location);
            ClientToScreen(impl_->windowWnd, &pt);
        }
        else {
            GetCursorPos(&pt);
        }

        TrackPopupMenuEx(menu.impl_->hmenu, uFlags, pt.x, pt.y, impl_->windowWnd, NULL);
    }

    BrowserWindow::~BrowserWindow() = default;
}
