#include "system_preferences.hpp"

namespace DeskGap {
    bool SystemPreferences::GetAndWatchDarkMode(std::function<void()>&&) {
        return false;
    }
}
