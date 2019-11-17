#include <Windows.h>
#include <cassert>
#include <thread>

#include "../system_preferences/system_preferences.h"

#pragma comment(lib, "advapi32.lib")

namespace {
    bool getIsDarkModeFromOpenedHKey(HKEY hKey) {
        DWORD result;
        unsigned long size;
        LSTATUS queryResult = RegQueryValueExA(hKey, "AppsUseLightTheme", NULL, NULL, (LPBYTE)&result, &size);
        if (queryResult != ERROR_SUCCESS) {
            return false;
        }
        return result == 0;
    }
}

namespace DeskGap {
    bool SystemPreferences::GetAndWatchDarkMode(std::function<void()>&& onDarkModeToggled) {
        HKEY hKey;
        LSTATUS openError = RegOpenKeyExA(
            HKEY_CURRENT_USER,
            "Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
            0, KEY_NOTIFY | KEY_QUERY_VALUE, &hKey
        );
        if (openError != ERROR_SUCCESS) {
            return false;
        }

        bool isDarkMode = getIsDarkModeFromOpenedHKey(hKey);

        new std::thread([isDarkMode, hKey, onDarkModeToggled = std::move(onDarkModeToggled)]() {
            bool lastIsDarkMode = isDarkMode;
            do {
                bool newIsDarkMode = getIsDarkModeFromOpenedHKey(hKey);
                if (lastIsDarkMode != newIsDarkMode) {
                    lastIsDarkMode = newIsDarkMode;
                    onDarkModeToggled();
                }
            }
            while (RegNotifyChangeKeyValue(hKey, FALSE, REG_NOTIFY_CHANGE_LAST_SET, 0, FALSE) == ERROR_SUCCESS);
        });

        return isDarkMode;
    }
}
