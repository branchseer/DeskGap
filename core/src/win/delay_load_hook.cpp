#include <Windows.h>

#include <delayimp.h>
#include <cstring>
#include <cassert>
#include "../lib_path.h"
#include "./util/wstring_utf8.h"
#include "./util/win32_check.h"
#include <filesystem>

namespace fs = std::filesystem;

static FARPROC WINAPI delayHook(unsigned int event, DelayLoadInfo* info) {
  if (event != dliNotePreLoadLibrary)
    return nullptr;

  if (_stricmp(info->szDll, "node.exe") == 0) {
    return (FARPROC)GetModuleHandleW(nullptr);
  }
  else if (_stricmp(info->szDll, "deskgap_winrt.dll") == 0) {
    static FARPROC winrtDllModule = nullptr;

    if (winrtDllModule == nullptr) {
      HMODULE currentDLLModule;
      check(GetModuleHandleExW(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        (LPCWSTR)&delayHook, &currentDLLModule
      ));

      wchar_t currentDllPath[MAX_PATH];
      check(GetModuleFileNameW(currentDLLModule, currentDllPath, sizeof(currentDllPath)));
      fs::path winrtDllPath = fs::path(currentDllPath).parent_path() / L"deskgap_winrt.dll";

      winrtDllModule = (FARPROC)LoadLibraryW(winrtDllPath.c_str());
      assert(winrtDllModule != nullptr);
    }
    return winrtDllModule;
  }

  return nullptr;
}

decltype(__pfnDliNotifyHook2) __pfnDliNotifyHook2 = delayHook;
