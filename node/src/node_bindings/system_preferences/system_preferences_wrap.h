#ifndef system_preferences_wrap_h
#define system_preferences_wrap_h

#include <napi.h>

namespace DeskGap {
    Napi::Object SystemPreferencesObject(const Napi::Env& env);
}

#endif
