//
// Created by Wang, Chi on 2019/11/14.
//
#include <Foundation/NSException.h>
#include <exception.hpp>
std::optional<DeskGap::Exception> DeskGap::TryCatch(std::function<void()>&& action) {
    @try {
        action();
    }
    @catch(NSException* e) {
        return Exception {
            e.name.UTF8String,
            e.reason.UTF8String
        };
    }
    return std::nullopt;
}
