//
// Created by 19948 on 2025/4/19.
//
#include <iostream>
#include <chrono>
#include <cassert>
#include "Thread.h"

void testThreadExecutesTask() {
    bool taskExecuted = false;

    Thread t([&]() {
                std::cout << "Thread task executing.\n";
                taskExecuted = true;
        }, "TestThread");

    t.start();
    t.join();

    assert(taskExecuted && "Task was not executed");
    std::cout << "[pass] Task executed\n";
}

void testThreadName() {
    Thread t([]() {}, "WorkerThread");
    assert(t.name() == "WorkerThread");
    std::cout << "[pass] testThreadName passed.\n";
}

void testThreadJoinWaitsForCompletion() {
    using namespace std::chrono;
    auto start = steady_clock::now();

    Thread t([]() {
        std::this_thread::sleep_for(seconds(2));
    }, "SleepThread");

    t.start();
    t.join();

    auto end = steady_clock::now();
    assert(duration_cast<seconds>(end - start).count() >= 2);
    std::cout << "[pass] testThreadJoinWaitsForCompletion passed.\n";
}

void testDoubleStartShouldBePrevented() {
    Thread t([]() {}, "SafeStart");

    t.start();
    try {
        t.start();  // �ڶ��ε��ã������Ӧ���쳣����Ч
        std::cout << "[error] testDoubleStartShouldBePrevented failed. No exception on second start.\n";
    } catch (...) {
        std::cout << "[pass] testDoubleStartShouldBePrevented passed.\n";
    }

    t.join();
}

void testDoubleJoinShouldBeHandled() {
    Thread t([]() {}, "JoinTwice");

    t.start();
    int result1 = t.join();
    int result2 = t.join();  // Ӧ�ú��Ի򷵻ش�����

    assert(result1 == 0); // ���� 0 �ǳɹ�
    assert(result2 != 0); // �ڶ��� join Ӧ���в�ͬ����
    std::cout << "[pass] testDoubleJoinShouldBeHandled passed.\n";
}

int main() {
    // testThreadExecutesTask();
    // testThreadName();
    // testThreadJoinWaitsForCompletion();
    // testDoubleStartShouldBePrevented();
    testDoubleJoinShouldBeHandled();
    return 0;

}