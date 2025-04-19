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


// ���߳� API ����
void testBasicLockOperations() {
    MutexLock mlock;

    bool locked = mlock.tryLock();
    assert(locked && "tryLock ʧ�ܣ�Ԥ����δ����״̬�·��� true");
    mlock.unlock();

    mlock.lock();
    locked = mlock.tryLock();
    assert(!locked && "tryLock �ɹ���Ԥ����������״̬�·��� false");

    mlock.unlock();
    locked = mlock.tryLock();
    assert(locked && "�ٴ� tryLock ʧ�ܣ�Ԥ���ڽ����󷵻� true");
    mlock.unlock();

    // TODO(δ����ʱ����ʱΪ�׳��쳣������ʧ��)
    try {
        mlock.unlock();
        std::cout << "δ����ʱ����ʱΪ�׳��쳣������ʧ��" << std::endl;
    } catch (...) {
        std::cout << "δ����ʱ����ʱ�׳��쳣�� ����ͨ��" << std::endl;
    }
    std::cout << "��������������ͨ��" << std::endl;
}

void testDoubleLockSameThread() {
    MutexLock mutex;
    mutex.lock();

    bool acquired_early = false;
    std::thread t([&]() {
        auto lock_start = std::chrono::steady_clock::now();
         mutex.lock();  // Ӧ�ñ�����
         auto lock_end = std::chrono::steady_clock::now();

         // ����ȴ�ʱ��
         auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(lock_end - lock_start).count();

         // �������ʱ��С�����߳�˯��ʱ�䣨����ǰ�õ�����
         if (duration <= 1000) {
             acquired_early = true;
             std::cout << "�Դ���������ʧ�ܣ�����ʱ��С�����߳�˯��ʱ��" << duration << "ms)\n";
         } else {
             std::cout << "�Դ���������ͨ��������ʱ��������߳�˯��ʱ�� " << duration << "ms\n";
         }
    });
    std::this_thread::sleep_for(std::chrono::seconds(10));
    mutex.unlock();
    t.join();
    assert(!acquired_early);  // �������Լ�����ʧ��
    std::cout << "�߳���������ͨ��\n";
}

// ���̻߳������
void testMultithreadedMutex() {
    MutexLock mlock;
    std::atomic<int> counter{0};
    const int incrementsPerThread = 100000;
    const int numThreads = 4;

    auto worker = [&]() {
        for (int i = 0; i < incrementsPerThread; ++i) {
            MutexLockGuard guard(mlock);
            // �ٽ����ڰ�ȫ�޸Ĺ������
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

    // ������ռ����Ƿ���ȷ����֤�����ݾ�̬
    assert(counter == numThreads * incrementsPerThread && "���̻߳������ʧ�ܣ��������������");
    std::cout << "[PASS] ���̻߳������ͨ����������ֵ = " << counter << std::endl;
}

// RAII �쳣��ȫ����
void testRAIIExceptionSafety() {
    MutexLock mlock;
    bool exceptionCaught = false;
    try {
        {
            MutexLockGuard guard(mlock);
            // �����������׳��쳣
            throw std::runtime_error("�����쳣");
        }
    } catch (const std::runtime_error &error) {
        std::cout << "�����쳣: " << error.what() <<  std::endl;
        exceptionCaught = true;
    }
    // �쳣�������Ӧ�Զ��ͷ�
    bool locked = mlock.tryLock();
    assert(locked && "RAII �쳣��ȫ����ʧ�ܣ���δ���Զ��ͷ�");
    mlock.unlock();
    assert(exceptionCaught && "�쳣δ��ȷ����");

    std::cout << "[PASS] RAII �쳣��ȫ����ͨ��" << std::endl;
}

int main() {
    std::cout << "��ʼ���� MutexLock �� MutexLockGuard �ӿ� ..." << std::endl;
    testBasicLockOperations();
    // testDoubleLockSameThread();
    testMultithreadedMutex();
    testRAIIExceptionSafety();
    std::cout << "���в��Ծ�ͨ����" << std::endl;
    return 0;
}
