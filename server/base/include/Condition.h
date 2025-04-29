#pragma once

#include <errno.h>
#include <pthread.h>
#include <pthread.h>
#include <time.h>
#include <cstdint>
#include "MutexLock.h"
#include "noncopyable.h"



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
    explicit Condition(MutexLock& _mutex) : mutex(mutex) {
        pthread_cond_init(&cond_, NULL);
    };
    ~Condition() = default;

    void wait() {
        pthread_cond_wait(&cond_, mutex.get());
    }

    bool waitForSeconds(int seconds) {
        struct timespec abstime;
        clock_gettime(CLOCK_REALTIME, &abstime);
        abstime.tv_sec += static_cast<time_t>(seconds);
        return ETIMEDOUT == pthread_cond_timedwait(&cond_, mutex.get(), &abstime);
    }

    void notify() {
        pthread_cond_signal(&cond_);
    }

    void notifyAll() {
        pthread_cond_broadcast(&cond_);
    }

private:
    MutexLock& mutex;
    pthread_cond_t cond_;
};