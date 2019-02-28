#include <functional>
#include <memory>
#include <string>
#include <optional>
#include <filesystem>
#include <cassert>

#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Storage.Streams.h>

#include "../webview/webview.h"
#include "webview_impl.h"
#include "../lib_path.h"

#pragma comment(lib, "WindowsApp")

namespace fs = std::filesystem;

using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Web;
using namespace winrt::Windows::Web::UI;
using namespace winrt::Windows::Web::UI::Interop;
using namespace winrt::Windows::Storage;


namespace {
    const wchar_t* const WebViewHostWndClassName = L"DeskGapWebViewHost";
    const winrt::hstring LocalContentIdentifier = L"DeskGapLocalContent";
}

namespace DeskGap {
    class WebView::Impl::StreamResolver : public winrt::implements<WebView::Impl::StreamResolver, IUriToStreamResolver> {
    private:
        std::optional<fs::path> folder_;
    public:
        void setFolder(std::optional<fs::path>&& folder) {
            folder_ = std::move(folder);
        }
        IAsyncOperation<Streams::IInputStream> UriToStreamAsync(Uri uri) {
            if (!folder_.has_value()) return nullptr;
            const wchar_t* wpath = uri.Path().c_str();
            while (*wpath == '/') ++wpath;

            std::string fullPath = (folder_.value() / winrt::to_string(wpath)).string();

            StorageFile file = co_await StorageFile::GetFileFromPathAsync(winrt::to_hstring(fullPath));
            Streams::IInputStream stream = co_await file.OpenReadAsync();
            return stream;
        }
    };
    WebView::Impl::Impl(): process(nullptr), webViewControl(nullptr), streamResolver(std::make_unique<StreamResolver>()) { }

    void WebView::Impl::InitControl(HWND parentWnd) {
        static bool isClassRegistered = false;
        if (!isClassRegistered) {
            isClassRegistered = true;
            WNDCLASSEXW wndClass{ };
            wndClass.cbSize = sizeof(WNDCLASSEXW);
            wndClass.hInstance = GetModuleHandleW(nullptr);
            wndClass.lpszClassName = WebViewHostWndClassName;
            wndClass.lpfnWndProc = [](HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) -> LRESULT {
                WebView::Impl* impl_ = reinterpret_cast<WebView::Impl*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
                if (impl_ != nullptr) {
                    if (msg == WM_SIZE) {
                        RECT rect { };
                        GetWindowRect(impl_->controlWnd, &rect);
                        impl_->webViewControl.Bounds(Rect(
                            0, 0,
                            static_cast<float>(rect.right-rect.left),
                            static_cast<float>(rect.bottom-rect.top)
                        ));
                        return 0;
                    }
                }
                return DefWindowProcW(hwnd, msg, wp, lp);
            };
            wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
            wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
            RegisterClassExW(&wndClass);
        }
        controlWnd = CreateWindowW(
            WebViewHostWndClassName, L"",
            WS_CHILD,
            0, 0, 0, 0, parentWnd, nullptr, nullptr, 0
        );
        SetWindowLongPtrW(controlWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));


        IAsyncOperation<WebViewControl> asyncOperation = process.CreateWebViewControlAsync(
            reinterpret_cast<int64_t>(controlWnd),
            { }
        );

        HANDLE actionCompleted = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
        HANDLE events[1] = { actionCompleted };
        DWORD handleCount = ARRAYSIZE(events);
        DWORD handleIndex = 0;

        asyncOperation.Completed([&](const auto&, const auto&) {
            SetEvent(actionCompleted);
        });
        
        CoWaitForMultipleHandles(0, INFINITE, handleCount, events, &handleIndex);
        CloseHandle(actionCompleted);

        assert(asyncOperation.Status() == AsyncStatus::Completed);

        webViewControl = asyncOperation.GetResults();
    }


    WebView::WebView(EventCallbacks&& callbacks): impl_(std::make_unique<Impl>()) {
        WebViewControlProcessOptions options;
        options.PrivateNetworkClientServerCapability(WebViewControlProcessCapabilityState::Enabled);
    
        impl_->process = WebViewControlProcess(options);
    }
    void WebView::LoadHTMLString(const std::string& html) {
        
    }

    void WebView::LoadLocalFile(const std::string& path) {
        fs::path fsPath(path);
        impl_->streamResolver->setFolder(fsPath.parent_path());

        Uri uri = impl_->webViewControl.BuildLocalStreamUri(LocalContentIdentifier, winrt::to_hstring(fsPath.filename().string()));
        impl_->webViewControl.NavigateToLocalStreamUri(uri, *(impl_->streamResolver));
    }

    void WebView::LoadRequest(
        const std::string& method,
        const std::string& urlString,
        const std::vector<HTTPHeader>& headers,
        const std::optional<std::string>& body
    ) {
        
    }

    void WebView::EvaluateJavaScript(const std::string& scriptString, std::optional<JavaScriptEvaluationCallback>&& optionalCallback) {
        
    }

    void WebView::SetDevToolsEnabled(bool enabled) { 

    }

    void WebView::Reload() {
        
    }

    WebView::~WebView() {
        SetWindowLongPtrW(impl_->controlWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(nullptr));
    }
}
