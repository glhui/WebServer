#pragma once

// std lib
#include <condition_variable>
#include <time.h>
// public interface
#include <MutexLock.h>
#include <noncopyable.h>

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