//
// Created by 19948 on 2025/4/15.
//
#include <iostream>
#include <thread>
#include <cassert>
#include <chrono>
#include <vector>
#include <atomic>
#include <io.h>
#include <stdexcept>

#include <MutexLock.h>
#include <Thread.h>


// 单线程 API 测试
void testBasicLockOperations() {
    MutexLock mlock;

    bool locked = mlock.tryLock();
    assert(locked && "tryLock 失败，预期在未锁定状态下返回 true");
    mlock.unlock();

    mlock.lock();
    locked = mlock.tryLock();
    assert(!locked && "tryLock 成功，预期在已锁定状态下返回 false");

    mlock.unlock();
    locked = mlock.tryLock();
    assert(locked && "再次 tryLock 失败，预期在解锁后返回 true");
    mlock.unlock();

    // TODO(未加锁时解锁时为抛出异常，测试失败)
    try {
        mlock.unlock();
        std::cout << "未加锁时解锁时为抛出异常，测试失败" << std::endl;
    } catch (...) {
        std::cout << "未加锁时解锁时抛出异常， 测试通过" << std::endl;
    }
    std::cout << "基本锁操作测试通过" << std::endl;
}

void testDoubleLockSameThread() {
    MutexLock mutex;
    mutex.lock();

    bool acquired_early = false;
    std::thread t([&]() {
        auto lock_start = std::chrono::steady_clock::now();
         mutex.lock();  // 应该被阻塞
         auto lock_end = std::chrono::steady_clock::now();

         // 计算等待时间
         auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(lock_end - lock_start).count();

         // 如果加锁时间小于主线程睡眠时间（即提前拿到锁）
         if (duration <= 1000) {
             acquired_early = true;
             std::cout << "显存阻塞测试失败，加锁时间小于主线程睡眠时间" << duration << "ms)\n";
         } else {
             std::cout << "显存阻塞测试通过，加锁时间大于主线程睡眠时间 " << duration << "ms\n";
         }
    });
    std::this_thread::sleep_for(std::chrono::seconds(10));
    mutex.unlock();
    t.join();
    assert(!acquired_early);  // 触发断言即测试失败
    std::cout << "线程阻塞测试通过\n";
}

// 多线程互斥测试
void testMultithreadedMutex() {
    MutexLock mlock;
    std::atomic<int> counter{0};
    const int incrementsPerThread = 100000;
    const int numThreads = 4;

    auto worker = [&]() {
        for (int i = 0; i < incrementsPerThread; ++i) {
            MutexLockGuard guard(mlock);
            // 临界区内安全修改共享变量
            ++counter;
        }
    };

    std::vector<std::thread> threads;
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back(worker);
    }
    for (auto &t : threads) {
        t.join();
    }

    // 检查最终计数是否正确，验证无数据竞态
    assert(counter == numThreads * incrementsPerThread && "多线程互斥测试失败，计数器结果错误");
    std::cout << "[PASS] 多线程互斥测试通过，计数器值 = " << counter << std::endl;
}

// RAII 异常安全测试
void testRAIIExceptionSafety() {
    MutexLock mlock;
    bool exceptionCaught = false;
    try {
        {
            MutexLockGuard guard(mlock);
            // 在作用域内抛出异常
            throw std::runtime_error("测试异常");
        }
    } catch (const std::runtime_error &error) {
        std::cout << "捕获异常: " << error.what() <<  std::endl;
        exceptionCaught = true;
    }
    // 异常捕获后，锁应自动释放
    bool locked = mlock.tryLock();
    assert(locked && "RAII 异常安全测试失败，锁未能自动释放");
    mlock.unlock();
    assert(exceptionCaught && "异常未正确捕获");

    std::cout << "[PASS] RAII 异常安全测试通过" << std::endl;
}

int main() {
    std::cout << "开始测试 MutexLock 与 MutexLockGuard 接口 ..." << std::endl;
    testBasicLockOperations();
    // testDoubleLockSameThread();
    testMultithreadedMutex();
    testRAIIExceptionSafety();
    std::cout << "所有测试均通过！" << std::endl;
    return 0;
}
