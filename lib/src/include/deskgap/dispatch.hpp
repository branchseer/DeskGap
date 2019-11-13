#ifndef DESKGAP_DISPATCH_HPP
#define DESKGAP_DISPATCH_HPP

#include <functional>
#include <string>

namespace DeskGap {
    void UISync(std::function<void()>&& action);   
    void UIAsync(std::function<void()>&& action);
}

#endif /* ui_dispatch_h */
