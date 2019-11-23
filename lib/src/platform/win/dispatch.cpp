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
    HANDLE actionCompleted = CreateEventExW(nullptr, nullptr, 0, SYNCHRONIZE | EVENT_MODIFY_STATE);
    DWORD handleIndex = 0;

    DispatchAsync([&]() {
        action();
        SetEvent(actionCompleted);
    });

    CoWaitForMultipleHandles(0, INFINITE, 1, &actionCompleted, &handleIndex);
    CloseHandle(actionCompleted);
}
