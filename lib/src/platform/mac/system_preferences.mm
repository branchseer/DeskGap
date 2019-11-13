#import <Cocoa/Cocoa.h>
#include "system_preferences.hpp"
#include "./util/string_convert.h"

namespace DeskGap {
    long SystemPreferences::GetUserDefaultInteger(const std::string& key) {
        return [[NSUserDefaults standardUserDefaults] integerForKey: NSStr(key)];
    }

    float SystemPreferences::GetUserDefaultFloat(const std::string& key) {
        return [[NSUserDefaults standardUserDefaults] floatForKey: NSStr(key)];
    }
    double SystemPreferences::GetUserDefaultDouble(const std::string& key) {
        return [[NSUserDefaults standardUserDefaults] doubleForKey: NSStr(key)];
    }
    std::string SystemPreferences::GetUserDefaultString(const std::string& key) {
        NSString* value = [[NSUserDefaults standardUserDefaults] stringForKey: NSStr(key)];
        if (!value) {
            return "";
        }
        return [value UTF8String];
    }
    bool SystemPreferences::GetUserDefaultBool(const std::string& key) {
        return [[NSUserDefaults standardUserDefaults] boolForKey: NSStr(key)];
    }
    std::string SystemPreferences::GetUserDefaultURL(const std::string& key) {
        NSURL* url = [[NSUserDefaults standardUserDefaults] URLForKey: NSStr(key)];
        if (!url) {
            return "";
        }
        return [[url absoluteString] UTF8String];
    }

    namespace {
        NSString* jsonStringify(id value) {
            if (value) {
                @try {
                    NSData* jsonData = [NSJSONSerialization dataWithJSONObject: value options: kNilOptions error: nil];
                    if (jsonData) {
                        return [[NSString alloc] initWithData: jsonData encoding: NSUTF8StringEncoding];
                    }
                }
                @catch(NSException*) { }
            }
            return nil;
        }
    }

    std::string SystemPreferences::GetUserDefaultDictionaryJSON(const std::string& key) {
        NSDictionary* dict = [[NSUserDefaults standardUserDefaults] dictionaryForKey: NSStr(key)];
        NSString* jsonString = jsonStringify(dict);
        if (!jsonString) {
            return "{}";
        }
        return [jsonString UTF8String];
    }
    std::string SystemPreferences::GetUserDefaultArrayJSON(const std::string& key) {
        NSArray* array = [[NSUserDefaults standardUserDefaults] arrayForKey: NSStr(key)];
        NSString* jsonString = jsonStringify(array);
        if (!jsonString) {
            return "[]";
        }
        return [jsonString UTF8String];
    }

    bool SystemPreferences::GetAndWatchDarkMode(std::function<void()>&& onDarkModeToggled) {
        if (@available(macOS 10.14, *)) {
            NSUserDefaults* defaults = [NSUserDefaults standardUserDefaults];
            auto isDarkMode = std::make_shared<bool>([[defaults stringForKey:@"AppleInterfaceStyle"] isEqualToString:@"Dark"]);

            auto sharedCallback = std::make_shared<std::function<void()>>(std::move(onDarkModeToggled));

            [[NSDistributedNotificationCenter defaultCenter]
                addObserverForName: @"AppleInterfaceThemeChangedNotification"
                object: nil queue: nil
                usingBlock: ^(NSNotification*) {
                    bool nowIsDarkNode = [[defaults stringForKey:@"AppleInterfaceStyle"] isEqualToString:@"Dark"];
                    if (nowIsDarkNode != *isDarkMode) {
                        *isDarkMode = nowIsDarkNode;
                        (*sharedCallback)();
                    }
                }
            ];
            return *isDarkMode;
        }
        else {
            return false;
        }
    }
}
