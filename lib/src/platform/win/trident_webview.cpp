#include <functional>
#include <memory>
#include <string>
#include <sstream>
#include <filesystem>
#include <atlbase.h>
#include <exdisp.h>
#include <exdispid.h>
#include <Windows.h>
#include <objbase.h>
#include <comutil.h>
#include <mshtml.h>
#include <mshtmhst.h>
#include <forward_list>

#include "webview.hpp"
#include "webview_impl.h"
#include "./util/win32_check.h"
#include "./util/wstring_utf8.h"

#pragma comment(lib, "comsuppw")

namespace fs = std::filesystem;

extern "C" {
    extern char BIN2CODE_DG_PRELOAD_TRIDENT_JS_CONTENT[];
    extern int BIN2CODE_DG_PRELOAD_TRIDENT_JS_SIZE;
}

namespace {
    const DISPID kExternalPostDispid = 0x1000;
    const wchar_t* const kExternalPostName = L"post";
    const DISPID kExternalDragDispid = 0x1001;
    const wchar_t* const kExternalDragName = L"drag";
    const DISPID kExternalErrorDispid = 0x1002;
    const wchar_t* const kExternalErrorName = L"error";

    class Connection {
    private:
        ATL::CComPtr<IConnectionPointContainer> connPointContainer_;
        REFIID connPoint_;
        DWORD cookie_;
    public:
        Connection(IDispatch* disp, REFIID connPoint, IDispatch* sink): connPoint_(connPoint) {            
            check(disp->QueryInterface(IID_IConnectionPointContainer, (void **)&connPointContainer_));

            ATL::CComPtr<IConnectionPoint> cp;
            check(connPointContainer_->FindConnectionPoint(connPoint, &cp));
            check(cp->Advise(sink, &cookie_));
        }
        ~Connection() {
            ATL::CComPtr<IConnectionPoint> cp;
            check(connPointContainer_->FindConnectionPoint(connPoint_, &cp));
            check(cp->Unadvise(cookie_));
        }
    };
}

namespace DeskGap {
    bool(*tridentWebViewTranslateMessage)(MSG* msg) = nullptr;
    struct TridentWebView::Impl:
        public WebView::Impl,
        public IOleClientSite,
        public IOleInPlaceSite,
        public IDocHostUIHandler,
        public IInternetSecurityManager,
        public IServiceProvider,
        public DWebBrowserEvents2 {
    private:
        static std::forward_list<Impl*> impls_;
        std::unique_ptr<Connection> dWebBrowserEvents2Connection_;
        ATL::CComPtr<IHTMLDocument2> GetCurrentHtmlDoc2(HRESULT* hrPtr) {
            HRESULT hr;

            ATL::CComPtr<IDispatch> disp;
            hr = webBrowser2->get_Document(&disp);

            if (SUCCEEDED(hr)) {
                if (disp == nullptr) {
                    *hrPtr = E_FAIL;
                    return nullptr;
                }

                ATL::CComPtr<IHTMLDocument2> htmlDoc2;

                hr = disp->QueryInterface(IID_IHTMLDocument2, (void**)&htmlDoc2);

                if (SUCCEEDED(hr)) {
                    if (htmlDoc2 == nullptr) {
                        *hrPtr = E_FAIL;
                        return nullptr;
                    }
                    return htmlDoc2;
                }
            }
            if (hrPtr != nullptr) {
                *hrPtr = hr;
            }
            return nullptr;
        }
    public:
        std::wstring preloadScript;

        std::wstring lastErrorMessage;
        HRESULT ExecuteJavaScript(const std::wstring& code) {
            HRESULT hr = S_OK;
            if (webBrowser2 == nullptr) {
                return E_FAIL;
            }

            ATL::CComPtr<IHTMLDocument2> htmlDoc2 = GetCurrentHtmlDoc2(&hr);

            if (FAILED(hr)) {
                return hr;
            }

            ATL::CComPtr<IDispatch> script;
            hr = htmlDoc2->get_Script(&script);
            if (FAILED(hr)) {
                return hr;
            }

            DISPID evalFuncionId;
            static ATL::CComBSTR evalFunctionName(L"eval");
            hr = script->GetIDsOfNames(IID_NULL, &evalFunctionName, 1, LOCALE_SYSTEM_DEFAULT, &evalFuncionId);
            if (FAILED(hr)) {
                return hr;
            }

            DISPPARAMS dispParams;
            memset(&dispParams, 0, sizeof(DISPPARAMS));

            ATL::CComVariant codeArg(code.c_str());
            dispParams.cArgs = 1;
            dispParams.rgvarg = &codeArg;
                
            EXCEPINFO execInfo;
            memset(&execInfo, 0, sizeof(EXCEPINFO));
            ATL::CComVariant vResult;
            UINT uArgError = (UINT)-1;

            hr = script->Invoke(evalFuncionId, IID_NULL, 0, DISPATCH_METHOD, &dispParams, &vResult, &execInfo, &uArgError);

            if (FAILED(hr)) {
                return hr;
            }

            return S_OK;
        }
        HWND containerWnd;
        ATL::CComPtr<IOleObject> oleObject;
        ATL::CComPtr<IWebBrowser2> webBrowser2;
        ATL::CComPtr<IOleInPlaceActiveObject> inPlaceActiveObject;



        WebView::EventCallbacks callbacks;

        Impl(WebView::EventCallbacks& callbacks):
            callbacks(std::move(callbacks)), containerWnd(nullptr) {

        }
        virtual void SetRect(int x, int y, int width, int height) override {
            if (webBrowser2 != nullptr) {
                webBrowser2->put_Left(x);
                webBrowser2->put_Top(y);
                webBrowser2->put_Width(width);
                webBrowser2->put_Height(height);
            }
        }

        virtual void InitWithParent(HWND containerWnd) override {
            if (tridentWebViewTranslateMessage == nullptr) {
                tridentWebViewTranslateMessage = [](MSG* msg) {
                    for (Impl* impl: impls_) {
                        if (IsChild(impl->containerWnd, msg->hwnd)) {
                            if (impl->inPlaceActiveObject->TranslateAcceleratorW(msg) == S_OK) {
                                return true;
                            }
                        }
                    }
                    return false;
                };
            }

            this->containerWnd = containerWnd;

            ATL::CComPtr<IClassFactory> classFactory;
            check(CoGetClassObject(CLSID_WebBrowser,
                CLSCTX_INPROC_SERVER | CLSCTX_INPROC_HANDLER, nullptr,
                IID_IClassFactory, (void **)&classFactory));

            check(classFactory->CreateInstance(nullptr, IID_IOleObject, (void**)&oleObject));

            check(oleObject->SetClientSite(this));
            check(OleSetContainedObject(oleObject, TRUE));
            RECT rect;
            check(GetWindowRect(containerWnd, &rect));
            check(oleObject->DoVerb(OLEIVERB_INPLACEACTIVATE, nullptr, this, -1, containerWnd, &rect));
            check(oleObject->QueryInterface(IID_IWebBrowser2, (void**)&webBrowser2));

            check(webBrowser2->QueryInterface(IID_IOleInPlaceActiveObject, (void**)&inPlaceActiveObject));

            dWebBrowserEvents2Connection_ = std::make_unique<Connection>(webBrowser2, DIID_DWebBrowserEvents2, this);

            impls_.push_front(this);

            webBrowser2->put_Silent(VARIANT_TRUE);

        };

        ~Impl() {
            impls_.remove(this);
            if (oleObject == nullptr && webBrowser2 == nullptr) {
                return;
            }

            dWebBrowserEvents2Connection_ = nullptr;

            webBrowser2->Stop();
            webBrowser2->ExecWB(OLECMDID_CLOSE, OLECMDEXECOPT_DONTPROMPTUSER, nullptr, nullptr);
            check(webBrowser2->put_Visible(VARIANT_FALSE));

            check(oleObject->DoVerb(OLEIVERB_HIDE, nullptr, static_cast<IOleClientSite*>(this), 0, containerWnd, nullptr));
            check(oleObject->Close(OLECLOSE_NOSAVE));
            check(OleSetContainedObject(oleObject, FALSE));
            check(oleObject->SetClientSite(nullptr));
            check(CoDisconnectObject(oleObject, 0));
        }


        // IInternetSecurityManager Begin
        STDMETHODIMP ProcessUrlAction(LPCWSTR url, DWORD action, BYTE *policy,
                                      DWORD policy_size, BYTE *context,
                                      DWORD context_size, DWORD flags,
                                      DWORD reserved) override {
            return S_OK;
        }

        STDMETHODIMP MapUrlToZone(LPCWSTR url, DWORD *zone, DWORD flags) override {
            *zone = URLZONE_TRUSTED;
            return S_OK;
        }

        STDMETHODIMP GetSecurityId(LPCWSTR url, BYTE *security_id,
                                   DWORD *security_id_size, DWORD_PTR reserved) override {
            return INET_E_DEFAULT_ACTION;
        }

        STDMETHODIMP GetSecuritySite(IInternetSecurityMgrSite **site) override {
            return INET_E_DEFAULT_ACTION;
        }

        STDMETHODIMP GetZoneMappings(DWORD zone, IEnumString **enum_string,
                                     DWORD flags) override {
            return INET_E_DEFAULT_ACTION;
        }

        STDMETHODIMP QueryCustomPolicy(LPCWSTR url, REFGUID guid_key, BYTE **policy,
                                       DWORD *policy_size, BYTE *context,
                                       DWORD context_size, DWORD reserved) override {
            return INET_E_DEFAULT_ACTION;
        }

        STDMETHODIMP SetSecuritySite(IInternetSecurityMgrSite *site) override {
            return INET_E_DEFAULT_ACTION;
        }

        STDMETHODIMP SetZoneMapping(DWORD zone, LPCWSTR pattern, DWORD flags) override {
            return INET_E_DEFAULT_ACTION;
        }
        // IInternetSecurityManager End

        STDMETHODIMP QueryService( REFGUID guidService, REFIID riid, void** ppvObject) override {
            if (guidService == SID_SInternetSecurityManager && riid == IID_IInternetSecurityManager) {
                *ppvObject = static_cast<IInternetSecurityManager*>(this);
                return S_OK;
            }
            return E_NOINTERFACE;
        }

        // IUnknown Begin
        HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void**ppvObject) override {
            if (riid == IID_IUnknown) {
                *ppvObject = static_cast<IOleClientSite*>(this);
            }
            else if (riid == IID_IServiceProvider) {
                *ppvObject = static_cast<IServiceProvider*>(this);
            }
            else if (riid == IID_IOleInPlaceSite)
            {
                *ppvObject = static_cast<IOleInPlaceSite*>(this);
            }
            else if (riid == IID_IDocHostUIHandler) {
                *ppvObject = static_cast<IDocHostUIHandler*>(this);
            }
            else if (riid == IID_IDispatch) {
                *ppvObject = static_cast<IDispatch*>(this);
            }
            else if (riid == DIID_DWebBrowserEvents2) {
                *ppvObject = static_cast<DWebBrowserEvents2*>(this);
            }
            else {
                return E_NOINTERFACE;
            }
            return S_OK;
        }
        ULONG STDMETHODCALLTYPE AddRef(void) override { return 1; }
        ULONG STDMETHODCALLTYPE Release(void) override { return 1; }
        // IUnknown End

        // IOleClientSite Begin
        HRESULT STDMETHODCALLTYPE SaveObject(void) override { return E_NOTIMPL; }
        HRESULT STDMETHODCALLTYPE GetMoniker(DWORD, DWORD, __RPC__deref_out_opt IMoniker**) override { return E_NOTIMPL; }
        HRESULT STDMETHODCALLTYPE GetContainer(__RPC__deref_out_opt IOleContainer**) override { return E_NOINTERFACE; }
        HRESULT STDMETHODCALLTYPE ShowObject(void) override { return S_OK; }
        HRESULT STDMETHODCALLTYPE OnShowWindow(BOOL fShow) override { return S_OK; }
        HRESULT STDMETHODCALLTYPE RequestNewObjectLayout(void) override { return E_NOTIMPL; }
        // IOleClientSite End

        // IOleWindow Begin
        HRESULT STDMETHODCALLTYPE GetWindow( __RPC__deref_out_opt HWND *phwnd) override {
            (*phwnd) = containerWnd;
            return S_OK;
        }
        HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(BOOL) override { return E_NOTIMPL; }
        // IOleWindow End

        // IOleInPlaceSite Begin
        HRESULT STDMETHODCALLTYPE CanInPlaceActivate(void) override { return S_OK; }
        HRESULT STDMETHODCALLTYPE OnInPlaceActivate(void) override { return S_OK; }
        HRESULT STDMETHODCALLTYPE OnUIActivate(void) override { return S_OK; }
        HRESULT STDMETHODCALLTYPE GetWindowContext(
            IOleInPlaceFrame **ppFrame, IOleInPlaceUIWindow **ppDoc,
            LPRECT lprcPosRect, LPRECT lprcClipRect,
            LPOLEINPLACEFRAMEINFO lpFrameInfo) override {
            *ppFrame = nullptr;
            *ppDoc = nullptr;
            lpFrameInfo->fMDIApp = FALSE;
            lpFrameInfo->hwndFrame = containerWnd;
            lpFrameInfo->haccel = nullptr;
            lpFrameInfo->cAccelEntries = 0;
            return S_OK;
        }
        // IOleInPlaceSite End

        // IOleWindow Begin
        HRESULT STDMETHODCALLTYPE Scroll(SIZE scrollExtant) override { return E_NOTIMPL; }
        HRESULT STDMETHODCALLTYPE OnUIDeactivate(BOOL fUndoable) override { return S_OK; }
        HRESULT STDMETHODCALLTYPE OnInPlaceDeactivate(void) override { return S_OK; }
        HRESULT STDMETHODCALLTYPE DiscardUndoState(void) override { return E_NOTIMPL; }
        HRESULT STDMETHODCALLTYPE DeactivateAndUndo(void) override { return E_NOTIMPL; }
        HRESULT STDMETHODCALLTYPE OnPosRectChange(LPCRECT lprcPosRect) override {
            ATL::CComPtr<IOleInPlaceObject> inplace;
            webBrowser2->QueryInterface(IID_IOleInPlaceObject, (void **)&inplace);
            inplace->SetObjectRects(lprcPosRect, lprcPosRect);
            return S_OK;
        }
        // IOleWindow End

        
        // IDocHostUIHandler Begin
        HRESULT STDMETHODCALLTYPE ShowContextMenu(DWORD dwID, POINT*, IUnknown*, IDispatch*) override {
            if (dwID == CONTEXT_MENU_TEXTSELECT || dwID == CONTEXT_MENU_CONTROL) {
                return S_FALSE; 
            }
            return S_OK;
        }
        HRESULT STDMETHODCALLTYPE GetHostInfo(DOCHOSTUIINFO* pInfo) override {
            pInfo->dwFlags |= DOCHOSTUIFLAG_NO3DBORDER | DOCHOSTUIFLAG_DPI_AWARE | DOCHOSTUIFLAG_THEME;
            return S_OK;
        }
        HRESULT STDMETHODCALLTYPE ShowUI(
            DWORD, IOleInPlaceActiveObject*,
            IOleCommandTarget*,
            IOleInPlaceFrame*,
            IOleInPlaceUIWindow*) override {
            return S_OK;
        } 
        
        HRESULT STDMETHODCALLTYPE HideUI(void) override { return S_OK; }
        HRESULT STDMETHODCALLTYPE UpdateUI(void) override { return S_OK; }
        HRESULT STDMETHODCALLTYPE EnableModeless(BOOL fEnable) override { return S_OK; }
        HRESULT STDMETHODCALLTYPE OnDocWindowActivate(BOOL fActivate) override { return S_OK; }
        HRESULT STDMETHODCALLTYPE OnFrameWindowActivate(BOOL fActivate) override { return S_OK; }
        HRESULT STDMETHODCALLTYPE ResizeBorder(LPCRECT,  IOleInPlaceUIWindow*, BOOL) override { return S_OK; }
        HRESULT STDMETHODCALLTYPE GetOptionKeyPath(LPOLESTR*, DWORD) override { return S_FALSE; }
        HRESULT STDMETHODCALLTYPE GetDropTarget(IDropTarget*, IDropTarget**) override { return E_NOTIMPL; }
        HRESULT STDMETHODCALLTYPE GetExternal(IDispatch** ppDispatch) override {
            *ppDispatch = static_cast<IDispatch *>(this);
            return S_OK;
        }
        HRESULT STDMETHODCALLTYPE TranslateUrl(DWORD, LPWSTR, LPWSTR *ppchURLOut) override {
            *ppchURLOut = nullptr;
            return S_FALSE;
        }
        HRESULT STDMETHODCALLTYPE FilterDataObject(IDataObject*, IDataObject** ppDORet) override {
            *ppDORet = nullptr;
            return S_FALSE;
        }
        HRESULT STDMETHODCALLTYPE TranslateAccelerator(LPMSG, const GUID*, DWORD) override {
            return S_FALSE;
        }
        
        
        // IDocHostUIHandler End

        // IDispatch Begin
        HRESULT STDMETHODCALLTYPE GetTypeInfoCount(UINT*) override { return S_OK; }
        HRESULT STDMETHODCALLTYPE GetTypeInfo(UINT, LCID,  ITypeInfo**) override { return S_OK; }
        HRESULT STDMETHODCALLTYPE GetIDsOfNames(REFIID, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId) override {
            if(cNames == 0 || rgszNames == nullptr || rgszNames[0] == nullptr || rgDispId == nullptr) {
		        return E_INVALIDARG;
	        }
            if (wcscmp(rgszNames[0], kExternalPostName) == 0) {
	            *rgDispId = kExternalPostDispid;
            }
            else if (wcscmp(rgszNames[0], kExternalDragName) == 0) {
                *rgDispId = kExternalDragDispid;
            }
            else if (wcscmp(rgszNames[0], kExternalErrorName) == 0) {
                *rgDispId = kExternalErrorDispid;
            }
	        return S_OK;
        }
        
        
        HRESULT STDMETHODCALLTYPE Invoke(DISPID dispIdMember, REFIID riid, LCID lcid,
            WORD wFlags, DISPPARAMS *pDispParams,
            VARIANT *pVarResult, EXCEPINFO *pExcepInfo,
            UINT *puArgErr) {

            if (dispIdMember == DISPID_NAVIGATECOMPLETE2) {
                {
                    IDispatch* dispatch = pDispParams->rgvarg[1].pdispVal;
                    CComPtr<IWebBrowser2> paramWebBrowser2;
                    check(dispatch->QueryInterface(IID_IWebBrowser2, (void**)&paramWebBrowser2));
                    if (paramWebBrowser2 != webBrowser2) return S_OK;
                }

                ExecuteJavaScript(preloadScript);
            }
            else if (dispIdMember == DISPID_DOCUMENTCOMPLETE) {
                IDispatch* dispatch = pDispParams->rgvarg[1].pdispVal;
                CComPtr<IWebBrowser2> paramWebBrowser2;
                check(dispatch->QueryInterface(IID_IWebBrowser2, (void**)&paramWebBrowser2));

                if (paramWebBrowser2 == webBrowser2) {
                    callbacks.didFinishLoad();
                }
            }
            else if (dispIdMember == DISPID_TITLECHANGE) {
                BSTR bstrTitle = pDispParams->rgvarg[0].bstrVal;
                std::wstring wTitle(bstrTitle, SysStringLen(bstrTitle));
                callbacks.onPageTitleUpdated(WStringToUTF8(wTitle.c_str()));
            }
            else if (dispIdMember == kExternalPostDispid) {
                BSTR bstrMessage = pDispParams->rgvarg[0].bstrVal;
                std::wstring wMessage(bstrMessage, SysStringLen(bstrMessage));
                callbacks.onStringMessage(WStringToUTF8(wMessage.c_str()));
            }
            else if (dispIdMember == kExternalDragDispid) {
                if (HWND windowWnd = GetAncestor(containerWnd, GA_ROOT); windowWnd != nullptr) {
                    if (SetFocus(windowWnd) != nullptr) {
                        SendMessage(windowWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
                    }
                }
            }
            else if (dispIdMember == kExternalErrorDispid) {
                BSTR bstrMessage = pDispParams->rgvarg[0].bstrVal;
                lastErrorMessage = std::wstring(bstrMessage, SysStringLen(bstrMessage));
            }
            return S_OK;
        }

        // IDispatch End
    };
    std::forward_list<TridentWebView::Impl*> TridentWebView::Impl::impls_;

    TridentWebView::TridentWebView(EventCallbacks&& callbacks, const std::string& preloadScript) {
        static std::string dgPreloadTridentJS(BIN2CODE_DG_PRELOAD_TRIDENT_JS_CONTENT, BIN2CODE_DG_PRELOAD_TRIDENT_JS_SIZE);
        auto tridentImpl = std::make_unique<Impl>(callbacks);

        std::ostringstream scriptStream;
        scriptStream << "(function(){\n" << dgPreloadTridentJS << preloadScript << "\n})();\n";

        std::string script = scriptStream.str();

        tridentImpl->preloadScript = UTF8ToWString(script.c_str());

        //impl_ for reference owning, and winrtImpl_ for method calling
        tridentImpl_ = tridentImpl.get();
        impl_ = std::move(tridentImpl);
        
    }

    void TridentWebView::LoadLocalFile(const std::string& path) {
        std::wstring wpath = UTF8ToWString(path.c_str());
        ATL::CComVariant flags(navNoHistory | navNoReadFromCache | navNoWriteToCache);
        tridentImpl_->webBrowser2->Navigate(_bstr_t(wpath.c_str()), &flags, nullptr, nullptr, nullptr);
    }

    void TridentWebView::LoadRequest(
        const std::string& method,
        const std::string& urlString,
        const std::vector<HTTPHeader>& headers,
        const std::optional<std::string>& body
    ) {
        std::wstring wURL = UTF8ToWString(urlString.c_str());
        ATL::CComVariant flags(navNoHistory | navNoReadFromCache | navNoWriteToCache);
        tridentImpl_->webBrowser2->Navigate(_bstr_t(wURL.c_str()), &flags, nullptr, nullptr, nullptr);
    }

    void TridentWebView::ExecuteJavaScript(const std::string& scriptString, std::optional<JavaScriptExecutionCallback>&& optionalCallback) {
        std::wstring wScriptString = UTF8ToWString(scriptString.c_str());
        HRESULT hr = tridentImpl_->ExecuteJavaScript(wScriptString);
        if (optionalCallback.has_value()) {
            std::optional<std::string> errorMessage;
            if (FAILED(hr)) {
                if (tridentImpl_->lastErrorMessage.empty()) {
                    errorMessage.emplace("Unknown Error");
                }
                else {
                    errorMessage.emplace(WStringToUTF8(tridentImpl_->lastErrorMessage.c_str()));
                }
            }
            (*optionalCallback)(std::move(errorMessage));
        }
    }

    void TridentWebView::SetDevToolsEnabled(bool enabled) { 
        
    }

    void TridentWebView::Reload() {
        //We don't use Refresh or Refresh2 here because they don't fire DISPID_NAVIGATECOMPLETE2 or DISPID_DOCUMENTCOMPLETE
        CComBSTR url;
        check(tridentImpl_->webBrowser2->get_LocationURL(&url));
        ATL::CComVariant flags(navNoHistory | navNoReadFromCache | navNoWriteToCache);
        tridentImpl_->webBrowser2->Navigate(url, &flags, nullptr, nullptr, nullptr);
    }

    TridentWebView::~TridentWebView() {

    }
}
