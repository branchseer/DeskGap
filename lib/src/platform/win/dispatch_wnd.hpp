//
// Created by patr0nus on 11/20/2019.
//
#ifndef WIN_DISPATCH_WND_HPP
#define WIN_DISPATCH_WND_HPP

#include <Windows.h>

namespace DeskGap {
    const UINT DG_DISPATCH_MSG = WM_APP + 1;
    extern HWND dispatchWindowWnd;
}

#endif
