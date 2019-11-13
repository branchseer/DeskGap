#ifndef DESKGAP_SHELL_H
#define DESKGAP_SHELL_H

#include <string>

namespace DeskGap {
    class Shell {
    public:
        static bool OpenExternal(const std::string& path);
    };
}

#endif
