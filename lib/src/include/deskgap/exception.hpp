//
// Created by Wang, Chi on 2019/11/14.
//

#ifndef DESKGAP_EXCEPTION_HPP
#define DESKGAP_EXCEPTION_HPP

#include <optional>
#include <string>

namespace DeskGap {
    struct Exception {
        std::string name;
        std::string message;
    };
    std::optional<Exception> TryCatch(std::function<void()>&& action);
}

#endif //DESKGAP_EXCEPTION_HPP
