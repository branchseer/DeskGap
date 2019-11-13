#import <Foundation/Foundation.h>
#include <utility>
#include "../dispatch/ui_dispatch_platform.h"

namespace {
    std::optional<DeskGap::PlatformException> ExecuteAction(const std::function<void()>& action) {
        @try {
            action();
        }
        @catch(NSException* e) {
            return DeskGap::PlatformException {
                [[e name] UTF8String],
                [[e reason] UTF8String]
            };
        }
        return std::nullopt;
    }
}

std::optional<DeskGap::PlatformException> DeskGap::UISyncPlatform(std::function<void()>&& action) {
    __block std::optional<PlatformException> platformException;
    dispatch_sync(dispatch_get_main_queue(), ^{
        platformException = ExecuteAction(action);
    });    
    return platformException;
}

void DeskGap::UIASyncPlatform(std::function<void()>&& action, std::function<void(std::optional<PlatformException>&&)> callback) {
    std::function<void(std::optional<PlatformException>&&)> capturedCallback = std::move(callback);
    std::function<void()> capturedAction = std::move(action);
    dispatch_async(dispatch_get_main_queue(), ^{
        std::optional<PlatformException> platformException = ExecuteAction(capturedAction);
        capturedCallback(std::move(platformException));
    });
}
