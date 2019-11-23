//
// Created by patr0nus on 11/19/2019.
//


#include "exception.hpp"

std::optional<DeskGap::Exception> DeskGap::TryCatch(std::function<void()>&& action) {
    try {
        action();
    }
    catch (const DeskGap::Exception& e) {
        return e;
    }
    return std::nullopt;
}
