//
// Created by 19948 on 2025/4/17.
//

#include <iostream>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <vector>
#include <chrono>
#include <cassert>

#include "Condition.h"


MutexLock mtx;
Condition condition(mtx);
bool ready = false;

std::mutex mutex;
std::condition_variable cv;

/**
 * @brief Basic test for `Condition` class.
 * 可能存在潜在bug，worker线程是否需要解锁
 */
void testBasicWaitNotify() {
    ready = false;
    std::thread worker([] {
        condition.wait(); // 工作线程等待锁
        std::cout << "Case 1 passed: worker resumed\n";
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 确保 worker 已等待
    {
        std::cout << "main thread set ready\n";
        ready = true;
    }
    condition.notify();
    worker.join();

}


/**
 * @brief Test the usage of `std::condition_variable`.
 * 通过标准库测试，条件变量的wait和notify必须搭配使用。
 * 无论条件变量的锁是否上锁，只要调用了wait就一定会陷入等待状态。
 */
void stdConditionTest() {
    std::thread worker([] {
        std::unique_lock<std::mutex> lock(mutex);
        std::cout << "worker thread get the mutex lock.\n";
        cv.wait(lock);
        std::cout << "worker thread running.\n";
    });

    std::this_thread::sleep_for(std::chrono::microseconds(10000));
    {
        std::unique_lock<std::mutex> lock(mutex);
        std::cout << "main thread get the mutex lock.\n";
    }
    cv.notify_one();
    worker.join();
}

void testNotifyAllThreads() {
    const int thread_count = 5;
    int wake_count = 0;
    auto waiter = [&]() {
       condition.wait();
        std::cout << "thread " << std::this_thread::get_id() << " resumed\n";
        ++wake_count;
    };
    std::vector<std::thread> threads(thread_count);
    for (auto& t : threads) t = std::thread(waiter);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    {
        MutexLockGuard lock(mtx);
        std::cout << "main thread set ready\n";
        ready = true;
    }
    condition.notifyAll();
    for (auto& t : threads) t.join();

    assert(wake_count == thread_count);
    std::cout << "Case 2 passed: all threads notified\n";
}

void test_notify_all_multiple_threads() {
    ready = false;
    const int thread_count = 5;
    int wake_count = 0;

    auto waiter = [&]() {
        std::unique_lock<std::mutex> lock(mutex);
        cv.wait(lock);
        ++wake_count;
    };

    std::vector<std::thread> threads(thread_count);
    for (auto& t : threads) t = std::thread(waiter);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    {
        std::lock_guard<std::mutex> lock(mutex);
        ready = true;
    }
    cv.notify_all();
    for (auto& t : threads) t.join();

    assert(wake_count == thread_count);
    std::cout << "Case 2 passed: all threads notified\n";
}

int main() {
    // testBasicWaitNotify();
    // stdConditionTest();
    testNotifyAllThreads();
    // test_notify_all_multiple_threads();
}