#ifndef WIN_PLATFORM_DATA
#define WIN_PLATFORM_DATA

#include <Windows.h>

namespace DeskGap {
    struct PlatformData {
        HWND dispatchWindowWnd;
        bool(*tridentWebViewTranslateMessage)(MSG* msg);
    };
}

#endif
