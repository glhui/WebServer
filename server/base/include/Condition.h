#pragma once

// std lib
#include <condition_variable>
#include <time.h>
// public interface
#include <MutexLock.h>
#include <noncopyable.h>



/**
 * @brief A condition variable wrapper class that provides mechanisms for
 *        thread synchronization.
 *
 * 本封装无法避免虚假唤醒现象
 * 虚假唤醒，线程会在notify之前被唤起，以加快速度，这在OS或者编译层面是允许的
 * 需要手动设置条件来避免这个问题。即std库中的wait(lock, predicate)
 * 推荐写法：
 * std::unique_lock<std::mutex> lock(mutex);
 * while (!ready) {
 *  cv.wait(lock);
 * }
 */

class Condition : noncopyable {
public:
    explicit Condition(MutexLock& mutex) : mutex_lock_(mutex) {};
    ~Condition() = default;

    void wait() {
        std::unique_lock<std::mutex> lock(mutex_lock_.getUnderlyingMutex());
        cond_.wait(lock);
    }

    bool waitForSeconds(int seconds) {
        std::unique_lock<std::mutex> lock(mutex_lock_.getUnderlyingMutex());
        return cond_.wait_for(lock, std::chrono::seconds(seconds)) == std::cv_status::no_timeout;
    }

    void notify() {
        cond_.notify_one();
    }

    void notifyAll() {
        cond_.notify_all();
    }

private:
    MutexLock& mutex_lock_;
    std::condition_variable cond_;
};