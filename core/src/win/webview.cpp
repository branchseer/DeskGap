#include <functional>
#include <memory>
#include <string>
#include <optional>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <cassert>
#include <cctype>

#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Web.Http.h>
#include <winrt/Windows.Web.Http.Headers.h>

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

    const wchar_t MessageNotifyStringPrefix = L'm';
    const wchar_t WindowDragNotifyStringPrefix = L'd';
    const wchar_t TitleUpdatedNotifyStringPrefix = L't';
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
    void WebView::Impl::PrepareScript() {
        static std::unique_ptr<winrt::hstring> preloadScript = nullptr;
        
        if (preloadScript == nullptr) {
            std::ostringstream scriptStream;
            fs::path scriptFolder = fs::path(LibPath()) / "dist" / "ui";

            for (const std::string& scriptFilename: { "preload_win.js", "preload.js" }) {
                winrt::hstring scriptFullPath = winrt::to_hstring((scriptFolder / scriptFilename).string());
                std::ifstream scriptFile(scriptFullPath.c_str(), std::ios::binary);
                scriptStream << scriptFile.rdbuf();
            }

            preloadScript = std::make_unique<winrt::hstring>(winrt::to_hstring(scriptStream.str()));
        }

        webViewControl.AddInitializeScript(*preloadScript);
    }

    void WebView::Impl::InitControl(HWND parentWnd) {
        static bool isClassRegistered = false;
        if (!isClassRegistered) {
            isClassRegistered = true;
            WNDCLASSEXW wndClass { };
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
        webViewControl.Settings().IsScriptNotifyAllowed(true);

        navigationCompletedRevoker = webViewControl.NavigationCompleted(
            winrt::auto_revoke, 
            [this](const auto&, const WebViewControlNavigationCompletedEventArgs& e) {
                if (e.IsSuccess()) {
                    this->callbacks.didStopLoading(std::nullopt);
                }
                else {
                    this->callbacks.didStopLoading(DeskGap::WebView::LoadingError {
                        static_cast<long>(e.WebErrorStatus()), "WebErrorStatus"
                    });
                }
            }
        );

        navigationStartingRevoker = webViewControl.NavigationStarting(
            winrt::auto_revoke,
            [this](const auto&, const auto&) {
                this->callbacks.didStartLoading();
            }
        );

        scriptNotifyRevoker = webViewControl.ScriptNotify(
            winrt::auto_revoke,
            [this](const auto&, const WebViewControlScriptNotifyEventArgs& e) {
                winrt::hstring notifyString = e.Value();
                wchar_t notifyStringPrefix = notifyString[0];
                std::string notifyContent = winrt::to_string(notifyString.c_str() + 1);
                switch (notifyStringPrefix) {
                case MessageNotifyStringPrefix: {
                    callbacks.onStringMessage(std::move(notifyContent));
                    break;
                }
                case WindowDragNotifyStringPrefix: {
                    if (HWND windowWnd = GetAncestor(controlWnd, GA_ROOT); windowWnd != nullptr) {
                        if (SetFocus(windowWnd) != nullptr) {
                            SendMessage(windowWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
                        }
                    }
                    break;
                }
                case TitleUpdatedNotifyStringPrefix: {
                    callbacks.onPageTitleUpdated(std::move(notifyContent));
                }
                default:
                    break;
                }
            }
        );
    }


    WebView::WebView(EventCallbacks&& callbacks): impl_(std::make_unique<Impl>()) {
        WebViewControlProcessOptions options;
        options.PrivateNetworkClientServerCapability(WebViewControlProcessCapabilityState::Enabled);
        impl_->callbacks = std::move(callbacks);
        impl_->process = WebViewControlProcess(options);
        //The real creation of WebViewControl happens in WebView::Impl::InitControl,
        //which is called by BrowserWindow, because it needs the handle of the window.
    }
    void WebView::LoadHTMLString(const std::string& html) {
        impl_->PrepareScript();
        impl_->streamResolver->setFolder(std::nullopt);

        impl_->webViewControl.NavigateToString(winrt::to_hstring(html));
    }

    void WebView::LoadLocalFile(const std::string& path) {
        impl_->PrepareScript();
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
        impl_->PrepareScript();
        impl_->streamResolver->setFolder(std::nullopt);

        Http::HttpRequestMessage httpMessage(
            Http::HttpMethod(winrt::to_hstring(method)),
            Uri(winrt::to_hstring(urlString))
        );
        if (body.has_value()) {
            httpMessage.Content(Http::HttpStringContent(winrt::to_hstring(*body)));
        }
        for (const HTTPHeader& header: headers) {
            static const std::string contentTypeField = "content-type";
            if (std::equal(
                header.field.begin(), header.field.end(),
                contentTypeField.begin(), contentTypeField.end(),
                [](char a, char b) { return std::tolower(a) == b;}
            )) {
                httpMessage.Content().Headers().ContentType(
                    Http::Headers::HttpMediaTypeHeaderValue(winrt::to_hstring(header.value))
                );
            }
            else {
                httpMessage.Headers().Append(
                    winrt::to_hstring(header.field),
                    winrt::to_hstring(header.value)
                );
            }
        }
        impl_->webViewControl.NavigateWithHttpRequestMessage(httpMessage);
    }

    void WebView::EvaluateJavaScript(const std::string& scriptString, std::optional<JavaScriptEvaluationCallback>&& optionalCallback) {
        IAsyncOperation<winrt::hstring> resultPromise = impl_->webViewControl.InvokeScriptAsync(
            L"eval", { winrt::to_hstring(scriptString) }
        );
        if (optionalCallback.has_value()) {
            resultPromise.Completed([
                callback = std::move(*optionalCallback)
            ](const auto& resultPromise, AsyncStatus status) {
                if (status == AsyncStatus::Completed) {
                    callback(false, winrt::to_string(resultPromise.GetResults()));
                }
                else {
                    winrt::hresult_error error(resultPromise.ErrorCode());
                    callback(true, "HRESULT " + std::to_string(error.code()) + ": " + winrt::to_string(error.message()));
                }
            });
        }
    }

    void WebView::SetDevToolsEnabled(bool enabled) { 

    }

    void WebView::Reload() {
        
    }

    WebView::~WebView() {
        SetWindowLongPtrW(impl_->controlWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(nullptr));
    }
}
