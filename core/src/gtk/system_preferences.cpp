#include "../system_preferences/system_preferences.h"

namespace DeskGap {
    bool SystemPreferences::GetAndWatchDarkMode(std::function<void()>&&) {
        return false;
    }
}
