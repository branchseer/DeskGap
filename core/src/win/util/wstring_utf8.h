#ifndef win_util_wstring_utf8_h
#define win_util_wstring_utf8_h

#include <Windows.h>
#include <string>
#include <utility>
#include <cassert>

namespace {
    std::string WStringToUTF8(const wchar_t* wstring) {
        DWORD utf8ByteCount = WideCharToMultiByte(
            CP_UTF8,
            0,
            wstring,
            -1,
            nullptr,
            0,
            nullptr,
            nullptr
        );
        assert(utf8ByteCount >= 1);
        std::string utf8String(utf8ByteCount - 1, '\0');
        DWORD result = WideCharToMultiByte(CP_UTF8,
            0,
            wstring,
            -1,
            utf8String.data(),
            utf8ByteCount,
            nullptr,
            nullptr
        );
        assert(result != 0);
        return std::move(utf8String);
    }
}

#endif
