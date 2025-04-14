#pragma once

// public interface
#include <noncopyable.h>
#include <Condition.h>
// std lib
#include <mutex> // TODO(add linux support)
#include <condition_variable>


class MutexLock : private noncopyable {
public:
    MutexLock() = default;
    ~MutexLock() = default;

    void lock() {
        mutex_.lock();
    }

    void unlock() {
        mutex_.unlock();
    }

    bool tryLock() {
        return mutex_.try_lock();
    }

    std::mutex & getUnderlyingMutex() {
        return mutex_;
    }

private:
    std::mutex mutex_;
};

class MutexLockGuard : noncopyable {
public:
    explicit MutexLockGuard(MutexLock& mutex)
        : mutex_(mutex) {
        mutex_.lock();
    }

    ~MutexLockGuard() {
        mutex_.unlock();
    }

private:
    MutexLock& mutex_;
};