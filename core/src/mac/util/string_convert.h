#ifndef util_string_convert_h
#define util_string_convert_h

#include <string>
#include <Foundation/Foundation.h>

namespace {
    inline NSString* NSStr(const std::string& utf8string) {
        return [NSString stringWithUTF8String: utf8string.c_str()];
    }

    inline std::string StdStr(const NSString* nsstr) {
        return std::string([nsstr UTF8String]);
    }
}

#endif
