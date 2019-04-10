#include <Windows.h>

#include <delayimp.h>
#include <cstring>
#include "./util/winrt.h"

static FARPROC WINAPI delayHook(unsigned int event, DelayLoadInfo* info) {
    if (event != dliNotePreLoadLibrary)
        return nullptr;

    if (_stricmp(info->szDll, "node.exe") == 0) {
        return (FARPROC)GetModuleHandleW(nullptr);
    }
    else if (_stricmp(info->szDll, "deskgap_winrt.dll") == 0) {
        return (FARPROC)DeskGap::GetWinRTDLLModule();
    }

    return nullptr;
}

decltype(__pfnDliNotifyHook2) __pfnDliNotifyHook2 = delayHook;
