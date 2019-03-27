#include <Windows.h>

namespace DeskGap {
	
	int GetWindowDpi(HWND windowWnd) {
		typedef UINT (WINAPI *GetDpiForWindowProc)(HWND);
		static GetDpiForWindowProc getDpiForWindow = (GetDpiForWindowProc)GetProcAddress(LoadLibraryW(L"user32.dll"), "GetDpiForWindow");

		if (getDpiForWindow != nullptr) { // Not avaliable in win7
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
	

	inline LONG To96Dpi(HWND windowWnd, LONG val) {
		int dpi = GetWindowDpi(windowWnd);
		return val / 96 >= LONG_MAX / dpi ? LONG_MAX: MulDiv(val, dpi, 96);
	}
	inline POINT To96Dpi(HWND windowWnd, POINT pt) {
		int dpi = GetWindowDpi(windowWnd);
		return {
			pt.x / 96 >= LONG_MAX / dpi ? LONG_MAX: MulDiv(pt.x, dpi, 96),
			pt.y / 96 >= LONG_MAX / dpi ? LONG_MAX: MulDiv(pt.y, dpi, 96)
		};
	}

	inline POINT From96Dpi(HWND windowWnd, POINT pt) {
		int dpi = GetWindowDpi(windowWnd);
		return {
			MulDiv(pt.x, 96, dpi),
			MulDiv(pt.y, 96, dpi)
		};
	}
}
