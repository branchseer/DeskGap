#ifndef DESKGAP_SHELL_HPP
#define DESKGAP_SHELL_HPP

#include <string>

namespace DeskGap {
    class Shell {
    public:
        static bool OpenExternal(const std::string& path);
        static bool ShowItemInFolder(const std::string& path);
    };
}

#endif
