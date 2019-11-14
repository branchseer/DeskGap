#ifndef shell_shell_wrap_h
#define shell_shell_wrap_h

#include <napi.h>

namespace DeskGap {
    Napi::Object ShellObject(const Napi::Env& env);
}

#endif
