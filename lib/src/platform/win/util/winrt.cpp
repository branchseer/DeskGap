#include <mutex>

#include "winrt.h"
#include "win32_check.h"
#include "winrt_webview.hpp"

static std::once_flag winrtDLLLoadOnceFlag;
static HMODULE winrtDLLModule;

HMODULE DeskGap::GetWinRTDLLModule() {
    std::call_once(winrtDLLLoadOnceFlag, []() {
        winrtDLLModule = LoadLibraryW(L"deskgap_winrt.dll");
    });
    return winrtDLLModule;
}
