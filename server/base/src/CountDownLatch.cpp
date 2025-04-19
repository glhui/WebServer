//
// Created by 19948 on 2025/4/14.
//

#include <CountDownLatch.h>
#include <iostream>

CountDownLatch::CountDownLatch(int count)
    : mutex_lock_(), condition_(mutex_lock_), count_(count) {}

void CountDownLatch::wait() {
    // mutex_lock_.lock();
    while (count_ > 0) {
        // mutex_lock_.unlock();
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
