//
// Created by patr0nus on 11/19/2019.
//


#include "exception.hpp"

std::optional<DeskGap::Exception> DeskGap::TryCatch(std::function<void()>&& action) {
    action();
    return std::nullopt;
}
