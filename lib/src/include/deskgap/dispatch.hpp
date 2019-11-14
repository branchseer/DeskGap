#ifndef DESKGAP_DISPATCH_HPP
#define DESKGAP_DISPATCH_HPP

#include <functional>

namespace DeskGap {
    void DispatchSync(std::function<void()>&& action);
    void DispatchAsync(std::function<void()>&& action);
}

#endif /* ui_dispatch_h */
