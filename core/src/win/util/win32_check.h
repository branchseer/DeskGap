#include <Windows.h>
#include <cassert>

namespace DeskGap {
	inline void check(HRESULT hresult) {
		assert(SUCCEEDED(hresult));
	}
	inline void check(BOOL boolResult) {
		assert(boolResult != 0);
	}
}
