#include <Windows.h>

#include "menu_impl.h"
#include "webview_impl.h"
#include "./BrowserWindow_impl.h"
#include "./util/wstring_utf8.h"
#include "./util/win32_check.h"
#include "./util/dpi.h"

namespace {
	const wchar_t* const BrowserWindowWndClassName = L"DeskGapBrowserWindow";
}

namespace DeskGap {
    BrowserWindow::Impl::Impl(const WebView& webView, EventCallbacks& callbacks): webView(webView), callbacks(std::move(callbacks)) {

    }

    BrowserWindow::BrowserWindow(const WebView& webView, EventCallbacks&& callbacks): impl_(std::make_unique<Impl>(webView, callbacks)) {
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
                        case WM_ACTIVATE: {
                            if (wp == WA_INACTIVE) {
                                browserWindow->impl_->callbacks.onBlur();
                            }
                            else {
                                browserWindow->impl_->callbacks.onFocus();
                            }
                            break;
                        }
                        case WM_CLOSE: {
                            browserWindow->impl_->callbacks.onClose();
                            return 0;
                        }
                        case WM_SIZE: {
                            RECT rect { };
                            GetClientRect(hwnd, &rect);
                            LONG width = rect.right - rect.left;
                            LONG height = rect.bottom - rect.top;
                            browserWindow->impl_->webView.impl_->SetRect(0, 0, width, height);
                            browserWindow->impl_->callbacks.onResize();
                            break;
                        }
                        case WM_MOVE: {
                            browserWindow->impl_->callbacks.onMove();
                            break;
                        }
                        case WM_GETMINMAXINFO: {
                            LPMINMAXINFO mmInfo = (LPMINMAXINFO)lp;
                            mmInfo->ptMaxTrackSize = To96Dpi(browserWindow->impl_->windowWnd, browserWindow->impl_->maxTrackSize);
                            mmInfo->ptMinTrackSize = To96Dpi(browserWindow->impl_->windowWnd, browserWindow->impl_->minTrackSize);
                            return 0;
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

        impl_->windowWnd = CreateWindowW(
            BrowserWindowWndClassName,
            L"", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
            CW_USEDEFAULT, 0, 0,
            nullptr, nullptr,
            GetModuleHandleW(nullptr),
            nullptr
        );
        SetWindowLongPtrW(impl_->windowWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

        webView.impl_->InitWithParent(impl_->windowWnd);
    }

    void BrowserWindow::Show() {
        ShowWindow(impl_->windowWnd, SW_SHOW);
        UpdateWindow(impl_->windowWnd);
        
        SetFocus(impl_->windowWnd);
    }

    namespace {
        void SetWindowButtonEnabled(HWND hwnd, LONG button, bool enabled) {
            LONG style = GetWindowLongW(hwnd, GWL_STYLE);
            if (enabled) {
                style |= button;
            }
            else {
                style &= ~button;
            }
            SetWindowLongW(hwnd, GWL_STYLE, style);
        }
    }

    void BrowserWindow::SetMaximizable(bool maximizable) {
        SetWindowButtonEnabled(impl_->windowWnd, WS_MAXIMIZEBOX, maximizable);
    }
    void BrowserWindow::SetMinimizable(bool minimizable) {
        SetWindowButtonEnabled(impl_->windowWnd, WS_MINIMIZEBOX, minimizable);
    }
    void BrowserWindow::SetResizable(bool resizable) {
        SetWindowButtonEnabled(impl_->windowWnd, WS_SIZEBOX, resizable);
    }
    void BrowserWindow::SetHasFrame(bool hasFrame) {
        SetWindowButtonEnabled(impl_->windowWnd, WS_BORDER, hasFrame);
    }

    void BrowserWindow::SetTitle(const std::string& utf8title) {
        std::wstring wtitle = UTF8ToWString(utf8title.c_str());
        SetWindowTextW(impl_->windowWnd, wtitle.c_str());
    }

    void BrowserWindow::SetClosable(bool closable) {
        UINT uEnable = closable ? (MF_BYCOMMAND | MF_ENABLED) : (MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
        EnableMenuItem(GetSystemMenu(impl_->windowWnd, FALSE), SC_CLOSE, uEnable);
    }

    void BrowserWindow::SetSize(int width, int height, bool animate) {
        POINT scaledSize = To96Dpi(impl_->windowWnd, { width, height });
        SetWindowPos(
            impl_->windowWnd, nullptr, 0, 0,
            scaledSize.x, scaledSize.y,
            SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOMOVE
        );
    }

    void BrowserWindow::SetMaximumSize(int width, int height) {
        impl_->maxTrackSize.x = (width == 0 ? LONG_MAX: width);
        impl_->maxTrackSize.y = (height == 0 ? LONG_MAX: height);
    }
    void BrowserWindow::SetMinimumSize(int width, int height) {
        impl_->minTrackSize.x = width;
        impl_->minTrackSize.y = height;
    }

    void BrowserWindow::SetPosition(int x, int y, bool animate) {
        POINT scaledPosition = To96Dpi(impl_->windowWnd, POINT { x, y });
        // UINT dpi = GetDpiForWindow(impl_->windowWnd);
        // LONG dpiScaledX = MulDiv(x, dpi, 96);
        // LONG dpiScaledY = MulDiv(y, dpi, 96);
        SetWindowPos(
            impl_->windowWnd, nullptr,
            scaledPosition.x, scaledPosition.y, 0, 0,
            SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOSIZE
        );
    }

    std::array<int, 2> BrowserWindow::GetSize() {
        RECT rect;
        check(GetWindowRect(impl_->windowWnd, &rect));
        POINT size = From96Dpi(impl_->windowWnd, { rect.right - rect.left, rect.bottom - rect.top });
        return { size.x, size.y };    }

    std::array<int, 2> BrowserWindow::GetPosition() {
        RECT rect;
        check(GetWindowRect(impl_->windowWnd, &rect));
        POINT position = From96Dpi(impl_->windowWnd, { rect.left, rect.top });
        return { position.x, position.y };
    }

    void BrowserWindow::Minimize() {
        ShowWindow(impl_->windowWnd, SW_MINIMIZE);
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
        if (iconPath.has_value()) {
            std::wstring wiconPath = UTF8ToWString(iconPath->c_str());

            impl_->appIcon = LoadImage(nullptr, wiconPath.c_str(), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_LOADFROMFILE);
            if (impl_->appIcon != nullptr)
            {
                SendMessage(impl_->windowWnd, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(impl_->appIcon));
            }

            impl_->windowIcon = LoadImage(nullptr, wiconPath.c_str(), IMAGE_ICON, GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON), LR_LOADFROMFILE);
            if (impl_->windowIcon != nullptr)
            {            
                SendMessage(impl_->windowWnd, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(impl_->windowIcon));
            }
        }
    }

    void BrowserWindow::Destroy() {
        if (impl_->appIcon != nullptr && impl_->windowIcon != nullptr)
        {
            SendMessage(impl_->windowWnd, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(nullptr));
            SendMessage(impl_->windowWnd, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(nullptr));
            DeleteObject(impl_->appIcon);
            DeleteObject(impl_->windowIcon);
        }
        SetWindowLongPtrW(impl_->windowWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(nullptr));
        DestroyWindow(impl_->windowWnd);
    }
    void BrowserWindow::Close() {

    }

    void BrowserWindow::PopupMenu(const Menu& menu, const std::array<int, 2>* location, int positioningItem,  std::function<void()>&& onClose) {
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
        onClose();
    }

    BrowserWindow::~BrowserWindow() = default;
}
