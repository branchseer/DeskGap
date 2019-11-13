#ifndef DESKGAP_SYSTEM_PREFERENCES_HPP
#define DESKGAP_SYSTEM_PREFERENCES_HPP

#include <string>
#include <memory>
#include <variant>
#include <utility>
#include <functional>

namespace DeskGap {
    class SystemPreferences {
    public:
    #ifdef __APPLE__
    static long GetUserDefaultInteger(const std::string& key);
    static float GetUserDefaultFloat(const std::string& key);
    static double GetUserDefaultDouble(const std::string& key);
    static bool GetUserDefaultBool(const std::string& key);
    static std::string GetUserDefaultString(const std::string& key);
    static std::string GetUserDefaultURL(const std::string& key);

    static std::string GetUserDefaultArrayJSON(const std::string& key);
    static std::string GetUserDefaultDictionaryJSON(const std::string& key);
    #endif

    static bool GetAndWatchDarkMode(std::function<void()>&& onDarkModeToggled);
    };
}

#endif
