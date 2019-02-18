#include <utility>
#include <mutex>
#include <condition_variable>
#include <functional>

#include <gtkmm.h>

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

    std::optional<DeskGap::PlatformException> executeAndCatch(const std::function<void()>& action) {
      try {
        action();
      }
      catch (const Glib::Error& gerror) {
        return DeskGap::PlatformException {
           Glib::QueryQuark(gerror.domain()).operator Glib::ustring(),
           gerror.what()
        };
      }
      return std::nullopt;
    }
}

std::optional<DeskGap::PlatformException> DeskGap::UISyncPlatform(std::function<void()>&& action) {
    Semaphore semaphore;
    std::optional<DeskGap::PlatformException> exception;
    Glib::signal_idle().connect([&]() {
      exception = executeAndCatch(action);
      semaphore.signal();
      return false;
    });
    semaphore.wait();

    return exception;
}

void DeskGap::UIASyncPlatform(std::function<void()>&& action, std::function<void(std::optional<PlatformException>&&)> callback) {
  Glib::signal_idle().connect([
    action = std::move(action), callback = std::move(callback)
  ]() {
    callback(executeAndCatch(action));
    return false;
  });
}
