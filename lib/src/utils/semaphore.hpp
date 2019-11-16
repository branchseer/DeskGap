//
// Created by Wang, Chi on 2019/11/15.
//

#ifndef DESKGAP_SEMAPHORE_HPP
#define DESKGAP_SEMAPHORE_HPP

#include <mutex>
#include <condition_variable>

namespace {
    class Semaphore {
    private:
        std::mutex mutex_;
        std::condition_variable cv_;
        int count_ = 0;
    public:
        inline void signal() {
            std::unique_lock<std::mutex> lock(mutex_);
            ++count_;
            cv_.notify_one();
        }

        inline void wait() {
            std::unique_lock<std::mutex> lock(mutex_);
            cv_.wait(lock, [this] { return this->count_ > 0; });
            --count_;
        }
    };
}


#endif //DESKGAP_SEMAPHORE_HPP
