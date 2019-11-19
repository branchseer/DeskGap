//
// Created by Wang, Chi on 2019/11/19.
//

#include "exception.hpp"
#include "glib_exception.h"

std::optional<DeskGap::Exception> DeskGap::TryCatch(std::function<void()>&& action) {
    try {
        action();
    }
    catch (const DeskGap::GlibException& glibException) {
        return DeskGap::Exception {
                glibException.domain(), glibException.message()
        };
    }
    return std::nullopt;
}
