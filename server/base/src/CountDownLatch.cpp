//
// Created by 19948 on 2025/4/14.
//

#include "CountDownLatch.h"
#include <iostream>

CountDownLatch::CountDownLatch(int count)
    : mutex_(), condition_(mutex_), count_(count) {}

/**
 * void CountDownLatch::wait() {
    // mutex_lock_.lock();
    while (count_ > 0) {
        // mutex_lock_.unlock();
        condition_.wait();
    }
}
 * 错误代码分析：
 * 该代码在查看count时，首先需要加锁
 * 错误1，如果没有进入循环，那么锁忘记释放
 * 错误2，如果进入循环，count_ > 0，当锁释放后，
 * 原来的wait代码是：
 * void wait() {
        std::unique_lock<std::mutex> lock(mutex_lock_.getUnderlyingMutex());
        cond_.wait(lock);
    } 
    只有释放了锁才能再次加锁，
    所以不得不调用mutex_lock_.unlock();来先解锁代码，显得比较愚蠢。
 * 原来的wait代码中，注释了lock与unlock部分，是为了避免错误1，导致整个程序死锁。
 * 
 * 而在linux上合法的原因是condition_.wait();
 * 这个api调用的是pthread_cond_wait(&cond_, mutex.get());
 * 他需要的就是一个已经加锁的mutex
 * 而windows上需要的是unique_lock，导致我们不得不先解锁在加锁。再传参
 */

void CountDownLatch::wait() { 
    MutexLockGuard lock(mutex_);
    
    while (count_ > 0) {
        condition_.wait();
    }
}

void CountDownLatch::countDown() {
    MutexLockGuard lock(mutex_);
    --count_;
    if (count_ == 0) {
        condition_.notifyAll();
    }
}
