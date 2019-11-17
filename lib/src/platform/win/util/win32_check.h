#ifndef win_util_win32_check_h
#define win_util_win32_check_h

#include <Windows.h>
#include <cassert>

namespace {
	inline void check(HRESULT hresult) {
		assert(SUCCEEDED(hresult));
	}
	inline void check(BOOL boolResult) {
		assert(boolResult != 0);
	}
	inline void check(DWORD dwordResult) {
		assert(dwordResult != 0);
	}
}

#endif
