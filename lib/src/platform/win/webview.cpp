#include "webview.hpp"
#include "webview_impl.h"
#include "./util/winrt.h"

namespace {
    //Reference: https://docs.microsoft.com/en-us/windows/win32/api/securitybaseapi/nf-securitybaseapi-checktokenmembership
    BOOL IsAdmin(VOID) {
        BOOL b;
        SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
        PSID AdministratorsGroup;
        b = AllocateAndInitializeSid(
                &NtAuthority,
                2,
                SECURITY_BUILTIN_DOMAIN_RID,
                DOMAIN_ALIAS_RID_ADMINS,
                0, 0, 0, 0, 0, 0,
                &AdministratorsGroup);
        if(b) {
            if (!CheckTokenMembership( NULL, AdministratorsGroup, &b)) {
                b = FALSE;
            }
            FreeSid(AdministratorsGroup);
        }
        return(b);
    }
}

namespace DeskGap {
    bool WebView::IsWinRTWebViewAvailable() {
        if (IsAdmin()) {
            return false;
        }
        if (GetWinRTDLLModule() == nullptr) {
            return false;
        }
        return WinRTWebView::IsAvailable();
    }
}
