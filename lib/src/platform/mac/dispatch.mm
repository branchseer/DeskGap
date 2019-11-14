#include <dispatch/dispatch.h>
#include <utility>
#include "dispatch.hpp"


void DeskGap::DispatchSync(std::function<void()>&& action) {
    dispatch_sync_f(dispatch_get_main_queue(), &action, [](void* context) {
        auto action = static_cast<std::function<void()>*>(context);
        (*action)();
    });
}
void DeskGap::DispatchAsync(std::function<void()>&& action) {
    dispatch_async_f(dispatch_get_main_queue(), new std::function(std::move(action)), [](void* context) {
        auto action = static_cast<std::function<void()>*>(context);
        (*action)();
        delete action;
    });
}
