#include <utility>
#include <mutex>
#include <condition_variable>
#include <functional>

#include <gtk/gtk.h>

#include "dispatch.hpp"
#include "./glib_exception.h"
#include "../../utils/semaphore.hpp"


namespace {
    using Action = std::function<void()>;

    void GIdleAdd(Action&& action) {
        g_idle_add([](void* data) -> gboolean {
            auto action = static_cast<Action*>(data);
            (*action)();
            delete action;
            return FALSE;
        }, new Action(std::move(action)));
    }
}


void DeskGap::DispatchSync(std::function<void()>&& action) {
    Semaphore semaphore;
    GIdleAdd([&]() {
        action();
        semaphore.signal();
    });
    semaphore.wait();
}

void DeskGap::DispatchAsync(std::function<void()>&& action) {
    GIdleAdd([
        action { std::move(action) }
    ]() {
        action();
    });
}
