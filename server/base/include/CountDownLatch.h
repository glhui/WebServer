#pragma once

// public interface
#include <noncopyable.h>
#include <Condition.h>
#include <MutexLock.h>

class CountDownLatch : noncopyable {
public:
    explicit CountDownLatch(int count);

    void wait();

    void countDown();

    int getCount() const;

private:
    mutable MutexLock mutex_lock_;
    Condition condition_;
    int count_;
};