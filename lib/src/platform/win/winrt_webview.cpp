#include <functional>
#include <memory>
#include <string>
#include <optional>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <cassert>
#include <cctype>

#include <Windows.h>
#include <objbase.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Web.UI.Interop.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Web.Http.h>
#include <winrt/Windows.Web.Http.Headers.h>
#include <winrt/Windows.Foundation.Metadata.h>

#include "winrt_webview.hpp"
#include "webview_impl.h"
#include "exception.hpp"

#pragma comment(lib, "WindowsApp")


extern "C" {
    extern char BIN2CODE_DG_PRELOAD_WINRT_JS_CONTENT[];
    extern int BIN2CODE_DG_PRELOAD_WINRT_JS_SIZE;
}

namespace fs = std::filesystem;

using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Web;
using namespace winrt::Windows::Web::UI;
using namespace winrt::Windows::Web::UI::Interop;
using namespace winrt::Windows::Storage;

namespace {
    const wchar_t* const WebViewHostWndClassName = L"DeskGapWinRTWebViewHost";
    const winrt::hstring LocalContentIdentifier = L"DeskGapLocalContent";

    const wchar_t MessageNotifyStringPrefix = L'm';
    const wchar_t WindowDragNotifyStringPrefix = L'd';
    const wchar_t TitleUpdatedNotifyStringPrefix = L't';

    std::unique_ptr<winrt::hstring> preloadScript;
}

namespace DeskGap {
    namespace {
        class StreamResolver : public winrt::implements<StreamResolver, IUriToStreamResolver> {
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
    }
    bool WinRTWebView::IsAvailable() {
        // WinRTWebView needs WebViewControl#AddInitializeScript, which is a method of IWebViewControl2, which is in UniversalApiContract 7.
        return winrt::Windows::Foundation::Metadata::ApiInformation::IsApiContractPresent(L"Windows.Foundation.UniversalApiContract", 7);
    }

    struct WinRTWebView::Impl: public WebView::Impl {
        HWND controlWnd;
        winrt::Windows::Web::UI::Interop::WebViewControlProcess process;
        winrt::Windows::Web::UI::Interop::WebViewControl webViewControl;

        winrt::Windows::Web::UI::Interop::WebViewControl::NavigationCompleted_revoker navigationCompletedRevoker;
        winrt::Windows::Web::UI::Interop::WebViewControl::NavigationStarting_revoker navigationStartingRevoker;
        winrt::Windows::Web::UI::Interop::WebViewControl::ScriptNotify_revoker scriptNotifyRevoker;

        StreamResolver streamResolver;

        WebView::EventCallbacks callbacks;

        Impl(WebView::EventCallbacks& callbacks):
            callbacks(std::move(callbacks)),
            process(nullptr), webViewControl(nullptr) {

        }

        void PrepareScript() {
            webViewControl.AddInitializeScript(*preloadScript);
        }

        virtual void SetRect(int x, int y, int width, int height) override {
            if (webViewControl == nullptr) return;
            webViewControl.Bounds(Rect(
                0, 0,
                width,
                height
            ));
        }

        virtual void InitWithParent(HWND parentWnd) override {
            controlWnd = parentWnd;

            IAsyncOperation<WebViewControl> asyncOperation = process.CreateWebViewControlAsync(
                reinterpret_cast<int64_t>(controlWnd),
                { }
            );

            HANDLE actionCompleted = CreateEventExW(nullptr, nullptr, 0, SYNCHRONIZE | EVENT_MODIFY_STATE);
            DWORD handleIndex = 0;

            asyncOperation.Completed([&](const auto&, const auto&) {
                SetEvent(actionCompleted);
            });

            CoWaitForMultipleHandles(0, INFINITE, 1, &actionCompleted, &handleIndex);
            CloseHandle(actionCompleted);

            if (asyncOperation.Status() != AsyncStatus::Completed) {
                winrt::hresult_error hrError(asyncOperation.ErrorCode());
                throw DeskGap::Exception { "HRESULT: " + std::to_string(hrError.code()), winrt::to_string(hrError.message()) };
            }

            webViewControl = asyncOperation.GetResults();
            webViewControl.Settings().IsScriptNotifyAllowed(true);

            navigationCompletedRevoker = webViewControl.NavigationCompleted(
                winrt::auto_revoke, 
                [this](const auto&, const WebViewControlNavigationCompletedEventArgs& e) {
                    this->callbacks.didFinishLoad();
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
                        break;
                    }
                    default:
                        break;
                    }
                }
            );
        }; 
    };

    WinRTWebView::WinRTWebView(EventCallbacks&& callbacks, const std::string& preloadScriptString) {
        std::string script;
        script.reserve(BIN2CODE_DG_PRELOAD_WINRT_JS_SIZE + preloadScriptString.size());
        script.assign(BIN2CODE_DG_PRELOAD_WINRT_JS_CONTENT, BIN2CODE_DG_PRELOAD_WINRT_JS_SIZE);
        script.append(preloadScriptString);
        preloadScript = std::make_unique<winrt::hstring>(winrt::to_hstring(script));

        auto winrtImpl = std::make_unique<Impl>(callbacks);

        //impl_ for reference owning, and winrtImpl_ for method calling
        winrtImpl_ = winrtImpl.get();
        impl_ = std::move(winrtImpl);

        WebViewControlProcessOptions options;
        options.PrivateNetworkClientServerCapability(WebViewControlProcessCapabilityState::Enabled);
        winrtImpl_->process = WebViewControlProcess(options);
        //The real creation of WebViewControl happens in WinRTWebView::Impl::InitWithParent,
        //which is called by BrowserWindow, because it needs the handle of the window.
    }


    void WinRTWebView::LoadLocalFile(const std::string& path) {
        winrtImpl_->PrepareScript();
        fs::path fsPath(path);
        winrtImpl_->streamResolver.setFolder(fsPath.parent_path());

        Uri uri = winrtImpl_->webViewControl.BuildLocalStreamUri(LocalContentIdentifier, winrt::to_hstring(fsPath.filename().string()));
        winrtImpl_->webViewControl.NavigateToLocalStreamUri(uri, winrtImpl_->streamResolver);
    }

    void WinRTWebView::LoadRequest(
        const std::string& method,
        const std::string& urlString,
        const std::vector<HTTPHeader>& headers,
        const std::optional<std::string>& body
    ) {
        winrtImpl_->PrepareScript();
        winrtImpl_->streamResolver.setFolder(std::nullopt);

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
        winrtImpl_->webViewControl.NavigateWithHttpRequestMessage(httpMessage);
    }

    void WinRTWebView::ExecuteJavaScript(const std::string& scriptString, std::optional<JavaScriptExecutionCallback>&& optionalCallback) {
        IAsyncOperation<winrt::hstring> resultPromise = winrtImpl_->webViewControl.InvokeScriptAsync(
            L"eval", { winrt::to_hstring(scriptString) }
        );
        if (optionalCallback.has_value()) {
            resultPromise.Completed([
                callback = std::move(*optionalCallback)
            ](const auto& resultPromise, AsyncStatus status) {
                if (status == AsyncStatus::Completed) {
                    callback(std::nullopt);
                }
                else {
                    winrt::hresult_error error(resultPromise.ErrorCode());
                    callback(std::make_optional<std::string>(
                        "HRESULT " + std::to_string(error.code()) + ": " + winrt::to_string(error.message())
                    ));
                }
            });
        }
    }

    void WinRTWebView::SetDevToolsEnabled(bool enabled) { 

    }

    void WinRTWebView::Reload() {
        winrtImpl_->PrepareScript();
        winrtImpl_->webViewControl.Refresh();
    }

    WinRTWebView::~WinRTWebView() {
        SetWindowLongPtrW(winrtImpl_->controlWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(nullptr));
    }
}
