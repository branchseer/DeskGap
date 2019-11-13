#ifndef DESKGAP_MAC_UTIL_STRING_CONVERT_T
#define DESKGAP_MAC_UTIL_STRING_CONVERT_T

#include <string>
#include <Foundation/Foundation.h>

namespace {
    inline NSString* NSStr(const std::string& utf8string) {
        return [NSString stringWithUTF8String: utf8string.c_str()];
    }

    inline std::string CXXStr(NSString* nsstr) {
        return std::string([nsstr UTF8String]);
    }
}

#endif
