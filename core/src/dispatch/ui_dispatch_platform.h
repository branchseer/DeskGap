#ifndef ui_dispatch_platform_h
#define ui_dispatch_platform_h

#include <functional>
#include <optional>
#include "ui_dispatch.h"

namespace DeskGap {
    struct PlatformException {
        std::string name;
        std::string message;
    };
    std::optional<PlatformException> UISyncPlatform(std::function<void()>&& action);
    void UIASyncPlatform(std::function<void()>&& action, std::function<void(std::optional<PlatformException>&&)> callback);
}

#endif /* ui_dispatch_platform_h */
