#include "dpi.h"

namespace DeskGap {
	int GetWindowDpi(HWND windowWnd) {
		typedef UINT (WINAPI *GetDpiForWindowProc)(HWND);
		static GetDpiForWindowProc getDpiForWindow = (GetDpiForWindowProc)GetProcAddress(LoadLibraryW(L"user32.dll"), "GetDpiForWindow");

		if (getDpiForWindow != nullptr) { // Not Available in win7
			if (int dpi = getDpiForWindow(windowWnd); dpi != 0) {
				return dpi;
			}
		}
		
		HDC screen = GetDC(windowWnd);
		if (screen == nullptr) {
			screen = GetDC(nullptr);
			if (screen == nullptr) {
				return 96;
			}
		}
		
		int xDpi = GetDeviceCaps(screen, LOGPIXELSX);
		int yDpi = GetDeviceCaps(screen, LOGPIXELSY);
		ReleaseDC(NULL, screen);
		return (xDpi + yDpi) / 2;
	}
}
