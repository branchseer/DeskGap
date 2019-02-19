#include <functional>
#include <memory>
#include <string>
#include <Windows.h>

#include "../webview/webview.h"
#include "webview_impl.h"
#include "./util/string_convert.h"
#include "../lib_path.h"

using UWPWebView = Microsoft::Toolkit::Forms::UI::Controls::WebView;
using Microsoft::Toolkit::Win32::UI::Controls::Interop::WinRT::WebViewControlScriptNotifyEventArgs;
using Microsoft::Toolkit::Win32::UI::Controls::Interop::WinRT::WebViewControlNavigationCompletedEventArgs;
using Microsoft::Toolkit::Win32::UI::Controls::Interop::WinRT::WebViewControlNavigationStartingEventArgs;
using Microsoft::Toolkit::Win32::UI::Controls::Interop::WinRT::IUriToStreamResolver;
using System::Threading::Tasks::Task;
using System::Text::Encoding;
using System::Windows::Forms::DockStyle;
using System::Windows::Forms::Form;
using namespace System;
using namespace System::IO;
using namespace System::Collections::Generic;
using namespace System::ComponentModel;


namespace DeskGap {
    namespace {
        ref class LocalFileStreamResolver: public IUriToStreamResolver {
        private:
            String^ folderPath;
        public:
            LocalFileStreamResolver(String^ folderPath) {
                this->folderPath = folderPath;
            }
            virtual Stream^ UriToStream(Uri^ uri) sealed {
                String^ filePath = Path::Combine(this->folderPath, uri->AbsolutePath->TrimStart('/'));
                return gcnew FileStream(filePath, FileMode::Open, FileAccess::Read);
            }
        };

        const wchar_t MessageNotifyStringPrefix = L'm';
        const wchar_t WindowDragNotifyStringPrefix = L'd';
        const wchar_t TitleUpdatedNotifyStringPrefix = L't';

        ref class UWPWebViewCallbackHandler {
        private:
            const WebView::EventCallbacks* const callbacks_;
        public:
            ~UWPWebViewCallbackHandler() {
                delete callbacks_;
            }
            UWPWebViewCallbackHandler(WebView::EventCallbacks* callbacks):
                callbacks_(new WebView::EventCallbacks(std::move(*callbacks))) {

            }
            void OnNavigationStarting(Object^, WebViewControlNavigationStartingEventArgs^ e) {
                callbacks_->didStartLoading();
            }
            void OnNavigationCompleted(Object^, WebViewControlNavigationCompletedEventArgs^ e) {
                if (e->IsSuccess) {
                    callbacks_->didStopLoading(std::nullopt);
                }
                else {
                    callbacks_->didStopLoading(DeskGap::WebView::LoadingError { static_cast<long>(e->WebErrorStatus), StdStr(e->WebErrorStatus.ToString()) });
                }
            }
            void OnScriptNotify(Object^ sender, WebViewControlScriptNotifyEventArgs^ eventArgs) {
                String^ notifyString = eventArgs->Value;
                wchar_t notifyStringPrefix = notifyString[0];
                switch (notifyStringPrefix) {
                case MessageNotifyStringPrefix: {
                    callbacks_->onStringMessage(StdStr(notifyString->Substring(1)));
                    break;
                }
                case WindowDragNotifyStringPrefix: {
                    UWPWebView^ webview = safe_cast<UWPWebView^>(sender);
                    Form^ form = webview->FindForm();
                    if (form != nullptr) {
                        form->Focus();
                        if (ReleaseCapture()) {
                            SendMessage(static_cast<HWND>(form->Handle.ToPointer()), WM_NCLBUTTONDOWN, HTCAPTION, 0);
                        }
                    }
                    break;
                }
                case TitleUpdatedNotifyStringPrefix: {
                    callbacks_->onPageTitleUpdated(StdStr(notifyString->Substring(1)));
                }
                default:
                    break;
                }
            }
        };
    }

    void WebView::Impl::PrepareScript() {
        static gcroot<String^> preloadScript = nullptr;
        if (static_cast<String^>(preloadScript) == nullptr) {
            List<String^>^ scripts = gcnew List<String^>();
            String^ scriptDir = Path::Combine(ClrStr(LibPath()), "dist", "ui");

            for each(String^ scriptFilename in gcnew array<String^> { "preload_win.js", "preload.js" }) {
                scripts->Add(File::ReadAllText(Path::Combine(scriptDir, scriptFilename), Encoding::UTF8));
            }

            preloadScript = String::Join("\n", scripts);
        }

        uwpWebView->AddInitializeScript(preloadScript);
    }


    WebView::WebView(EventCallbacks&& callbacks): impl_(std::make_unique<Impl>()) {
        UWPWebView^ uwpWebView = gcnew UWPWebView();
        impl_->uwpWebView = uwpWebView;

        ((ISupportInitialize^)uwpWebView)->BeginInit();
        uwpWebView->Dock = DockStyle::Fill;
        ((ISupportInitialize^)uwpWebView)->EndInit();

        
        uwpWebView->IsScriptNotifyAllowed = true;

        UWPWebViewCallbackHandler^ handler = gcnew UWPWebViewCallbackHandler(&callbacks);
        uwpWebView->ScriptNotify += gcnew EventHandler<WebViewControlScriptNotifyEventArgs^>(handler, &UWPWebViewCallbackHandler::OnScriptNotify);
        uwpWebView->NavigationStarting += gcnew EventHandler<WebViewControlNavigationStartingEventArgs^>(handler, &UWPWebViewCallbackHandler::OnNavigationStarting);
        uwpWebView->NavigationCompleted += gcnew EventHandler<WebViewControlNavigationCompletedEventArgs^>(handler, &UWPWebViewCallbackHandler::OnNavigationCompleted);

        // impl_->PrepareScript();
    }
    void WebView::LoadHTMLString(const std::string& html) {
        impl_->PrepareScript();
        impl_->uwpWebView->NavigateToString(ClrStr(html));
    }

    void WebView::LoadLocalFile(const std::string& path) {
        impl_->PrepareScript();
        String^ clrPath = ClrStr(path);
        String^ fileName = Path::GetFileName(clrPath);
        String^ folder = Path::GetDirectoryName(clrPath);
        impl_->uwpWebView->NavigateToLocalStreamUri(gcnew Uri(fileName, UriKind::Relative), gcnew LocalFileStreamResolver(folder));
    }

    void WebView::LoadRequest(
        const std::string& method,
        const std::string& urlString,
        const std::vector<HTTPHeader>& headers,
        const std::optional<std::string>& body
    ) {
        using System::Net::Http::HttpMethod;
        using System::Collections::Generic::KeyValuePair;

        Uri^ uri = gcnew Uri(ClrStr(urlString));
        impl_->PrepareScript();

        array<KeyValuePair<String^, String^>>^ clrHeaders = nullptr;
        if (!headers.empty()) {
            clrHeaders = gcnew array<KeyValuePair<String^, String^>>(headers.size());
            for (int i = 0; i < headers.size(); ++i) {
                clrHeaders[i] = KeyValuePair<String^, String^>(
                    ClrStr(headers[i].field),
                    ClrStr(headers[i].value)
                );
            }
        }
        Console::WriteLine(uri->ToString());
        impl_->uwpWebView->Navigate(uri, gcnew HttpMethod(ClrStr(method)), body.has_value() ? ClrStr(*body): nullptr, clrHeaders);
    }

    namespace {
        ref class InvokeScriptAsyncResultHandler {
        private:
            WebView::JavaScriptEvaluationCallback* callback_;
        public:
            ~InvokeScriptAsyncResultHandler() {
                delete callback_;
            }
            InvokeScriptAsyncResultHandler(WebView::JavaScriptEvaluationCallback* callback):
                callback_(new WebView::JavaScriptEvaluationCallback(std::move(*callback))) {
            }
            void OnScriptResult(Task<String^>^ task) {
                if (task->Exception != nullptr) {
                    Exception^ exception = task->Exception->InnerException;
                    if (exception == nullptr) {
                        exception = task->Exception;
                    }
                    (*callback_)(true, StdStr(exception->ToString()));
                }
                else {
                    (*callback_)(false, StdStr(task->Result));
                }
            }
        };
    }

    void WebView::EvaluateJavaScript(const std::string& scriptString, std::optional<JavaScriptEvaluationCallback>&& optionalCallback) {
        Task<String^>^ task = impl_->uwpWebView->InvokeScriptAsync("eval", ClrStr(scriptString));
        if (optionalCallback.has_value()) {
            JavaScriptEvaluationCallback callback = std::move(*optionalCallback);
            task->ContinueWith(gcnew Action<Task<String^>^>(
                gcnew InvokeScriptAsyncResultHandler(&callback),
                &InvokeScriptAsyncResultHandler::OnScriptResult
            ));
        }
    }

    void WebView::SetDevToolsEnabled(bool enabled) { 

    }

    void WebView::Reload() {
        impl_->PrepareScript();
        impl_->uwpWebView->Refresh();
    }

    WebView::~WebView() = default;
}
