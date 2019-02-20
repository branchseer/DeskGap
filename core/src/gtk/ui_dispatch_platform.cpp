#include <utility>
#include <mutex>
#include <condition_variable>
#include <functional>

#include <gtk/gtk.h>

#include "../dispatch/ui_dispatch_platform.h"

namespace {
    using Action = std::function<void()>;
    class Semaphore {
    private:
        std::mutex mutex_;
        std::condition_variable cv_;
        int count_ = 0;
    public:
        void signal() {
            std::unique_lock<std::mutex> lock(mutex_);
            ++count_;
            cv_.notify_one();
        }

        void wait() {
            std::unique_lock<std::mutex> lock(mutex_);
            cv_.wait(lock, [this] { return this->count_ > 0; });
            --count_;
        }
    };

    std::optional<DeskGap::PlatformException> executeAndCatch(const std::function<void()>& action) {
        action();
        return std::nullopt;
    }

    void GIdleAdd(Action&& action) {
        g_idle_add([](void* data) -> gboolean {
            Action* action = static_cast<Action*>(data);
            (*action)();
            delete action;
            return FALSE;
        }, new Action(std::move(action)));
    }
}

std::optional<DeskGap::PlatformException> DeskGap::UISyncPlatform(Action&& action) {
    Semaphore semaphore;
    std::optional<DeskGap::PlatformException> exception;
    GIdleAdd([&]() {
        exception = executeAndCatch(action);
        semaphore.signal();
    });
    semaphore.wait();
    return exception;
}

void DeskGap::UIASyncPlatform(Action&& action, std::function<void(std::optional<PlatformException>&&)> callback) {
  GIdleAdd([
    action = std::move(action), callback = std::move(callback)
  ]() {
    callback(executeAndCatch(action));
    return false;
  });
}
