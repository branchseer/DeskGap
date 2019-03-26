#include <Windows.h>
#include <combaseapi.h>
#include "../dispatch/ui_dispatch_platform.h"
#include "./platform_data.h"
#include "../platform_data.h"
#include "util/wstring_utf8.h"

//#include <winrt/base.h>

//#pragma comment(lib, "WindowsApp")

namespace {
   std::optional<DeskGap::PlatformException> ExecuteAction(const std::function<void()>& action) {
      //try {
         action();
      //}
      //catch (const winrt::hresult_error& winrtError) {
      //   DeskGap::PlatformException platformException {
      //      "HRESULT " + std::to_string(winrtError.code()),
      //      winrt::to_string(winrtError.message())
      //   };
      //   return platformException;
      //}
      return std::nullopt;
   }
}

std::optional<DeskGap::PlatformException> DeskGap::UISyncPlatform(std::function<void()>&& action) {
   HWND dispatcherWnd = static_cast<PlatformData*>(GetPlatformData())->dispatchWindowWnd;

   std::optional<DeskGap::PlatformException> exception;

   HANDLE actionCompleted = CreateEventExW(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
   HANDLE events[1] = { actionCompleted };
   DWORD handleCount = ARRAYSIZE(events);
   DWORD handleIndex = 0;

   PostMessageW(
      dispatcherWnd,
      WM_APP + 1, 0,
      reinterpret_cast<LPARAM>(new std::function<void()>([&]() {
         exception = ExecuteAction(action);
         SetEvent(actionCompleted);
      }))
   );

   CoWaitForMultipleHandles(0, INFINITE, handleCount, events, &handleIndex);
   CloseHandle(actionCompleted);

   return exception;
}

void DeskGap::UIASyncPlatform(std::function<void()>&& action, std::function<void(std::optional<PlatformException>&&)> callback) {
   HWND dispatcherWnd = static_cast<PlatformData*>(GetPlatformData())->dispatchWindowWnd;
   
   PostMessageW(
      dispatcherWnd,
      WM_APP + 1, 0,
      reinterpret_cast<LPARAM>(new std::function<void()>([
         action = std::move(action),
         callback = std::move(callback)
      ]() {
         callback(ExecuteAction(action));
      }))
   );
}
