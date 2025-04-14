//
// Created by 19948 on 2025/4/14.
//

#include <CountDownLatch.h>

CountDownLatch::CountDownLatch(int count)
    : mutex_lock_(), condition_(mutex_lock_), count_(count) {}

void CountDownLatch::wait() {
    MutexLockGuard lock(mutex_lock_);
    while (count_ > 0) {
        condition_.wait();
    }
}

void CountDownLatch::countDown() {
    MutexLockGuard lock(mutex_lock_);
    --count_;
    if (count_ == 0) {
        condition_.notifyAll();
    }
}
