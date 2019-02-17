#include <utility>
#include <mutex>
#include <condition_variable>
#include <functional>

#include <gtk/gtk.h>

#include "../dispatch/ui_dispatch_platform.h"

namespace {
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
}

std::optional<DeskGap::PlatformException> DeskGap::UISyncPlatform(std::function<void()>&& action) {
    struct SyncDispatchData {
        std::function<void()> action;
        Semaphore semaphore;
        std::optional<DeskGap::PlatformException> exception;
    };
    SyncDispatchData data { std::move(action), Semaphore(), std::nullopt };
    g_idle_add([](void* data) -> gboolean {
        SyncDispatchData* dispatchData = static_cast<SyncDispatchData*>(data);
        dispatchData->action();
        dispatchData->semaphore.signal();
        return FALSE;
    }, &data);

    data.semaphore.wait();

    return data.exception;
}

void DeskGap::UIASyncPlatform(std::function<void()>&& action, std::function<void(std::optional<PlatformException>&&)> callback) {
    
}
