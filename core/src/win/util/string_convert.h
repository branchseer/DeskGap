#ifndef util_string_convert_h
#define util_string_convert_h

#include <string>

namespace {
    inline System::String^ ClrStr(const std::string& utf8string) {
        return gcnew System::String(
            utf8string.data(), 0, utf8string.size(), System::Text::Encoding::UTF8
        );
    }

    inline std::string StdStr(System::String^ clrString) {
        array<unsigned char>^ bytes = System::Text::Encoding::UTF8->GetBytes(clrString);
        if (bytes->Length == 0) {
            return { };
        }
        else {
            pin_ptr<System::Byte> pinned = &bytes[0];
            unsigned char* cstr = pinned;
            return reinterpret_cast<char*>(cstr);
        }
    }
}
#endif
