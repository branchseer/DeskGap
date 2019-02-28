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

    std::wstring UTF8ToWString(const char* utf8) {
        DWORD wbyteCount = MultiByteToWideChar(
            CP_UTF8,
            0,
            utf8,
            -1,
            nullptr,
            0
        );

        assert(wbyteCount >= 1);
        
        std::wstring wstring(wbyteCount - 1, L'\0');
        DWORD result = MultiByteToWideChar(CP_UTF8,
            0,
            utf8,
            -1,
            wstring.data(),
            wbyteCount
        );
        assert(result != 0);
        return std::move(wstring);

    }
}

#endif
