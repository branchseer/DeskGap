#ifndef win_util_dpi_h
#define win_util_dpi_h

#include <Windows.h>
#include <limits>

namespace DeskGap {
	int GetWindowDpi(HWND windowWnd);

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

#endif
