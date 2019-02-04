#ifndef shell_h
#define shell_h

#include <string>

namespace DeskGap {
    class Shell {
    public:
        static bool OpenExternal(const std::string& path);
    };
}

#endif
