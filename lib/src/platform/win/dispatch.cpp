#include <Windows.h>
#include <combaseapi.h>
#include <utility>
#include "dispatch.hpp"
#include "dispatch_wnd.hpp"

void DeskGap::DispatchAsync(std::function<void()>&& action) {

    PostMessageW(
        dispatchWindowWnd,
        DG_DISPATCH_MSG, 0,
        reinterpret_cast<LPARAM>(new std::function<void()>([
            action { std::move(action) }
        ]() {
            action();
        }))
    );
}

void DeskGap::DispatchSync(std::function<void()>&& action) {
    HANDLE actionCompleted = CreateEventExW(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
    HANDLE events[1] = { actionCompleted };
    DWORD handleCount = ARRAYSIZE(events);
    DWORD handleIndex = 0;

    DispatchAsync([&]() {
        action();
        SetEvent(actionCompleted);
    });

    CoWaitForMultipleHandles(0, INFINITE, handleCount, events, &handleIndex);

    CloseHandle(actionCompleted);
}
