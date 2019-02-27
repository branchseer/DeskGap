#include <Windows.h>
#include "../dispatch/ui_dispatch_platform.h"
#include "../platform_data.h"

namespace {
   std::optional<DeskGap::PlatformException> ExecuteAction(const std::function<void()>& action) {
      action();
      return std::nullopt;
   }
}

std::optional<DeskGap::PlatformException> DeskGap::UISyncPlatform(std::function<void()>&& action) {
   static DWORD mainThreadId = *static_cast<DWORD*>(PlatformData());

   std::optional<DeskGap::PlatformException> exception;

   HANDLE actionCompleted = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
   HANDLE events[1] = { actionCompleted };
   DWORD handleCount = ARRAYSIZE(events);
   DWORD handleIndex = 0;

   PostThreadMessageW(
      mainThreadId,
      WM_APP, 0,
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
   static DWORD mainThreadId = *static_cast<DWORD*>(PlatformData());
   PostThreadMessageW(
      mainThreadId,
      WM_APP, 0,
      reinterpret_cast<LPARAM>(new std::function<void()>([
         action = std::move(action),
         callback = std::move(callback)
      ]() {
         callback(ExecuteAction(action));
      }))
   );
}
