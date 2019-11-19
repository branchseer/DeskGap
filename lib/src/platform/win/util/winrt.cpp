#include "winrt.h"

#include "win32_check.h"
#include <filesystem>
#include "winrt_webview.hpp"

HMODULE DeskGap::GetWinRTDLLModule() {
    namespace fs = std::filesystem;

    static bool firstRun = true;
    static HMODULE winrtDLLModule;
    if (firstRun) {
        firstRun = false;

        HMODULE currentDLLModule;
        check(GetModuleHandleExW(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            (LPCWSTR)&GetWinRTDLLModule, &currentDLLModule
        ));

        wchar_t currentDLLPath[MAX_PATH];
        check(GetModuleFileNameW(currentDLLModule, currentDLLPath, sizeof(currentDLLPath)));
        fs::path winrtDllPath = fs::path(currentDLLPath).parent_path() / L"deskgap_winrt.dll";

        winrtDLLModule = LoadLibraryW(winrtDllPath.c_str());
    }
    return winrtDLLModule;
}
