//
// Created by 19948 on 2025/4/19.
//


#include <cassert>
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <condition_variable>
#include "CountDownLatch.h"


MutexLock printLock;
void testCountDownLatch() {
  const int workerCount = 5;
  CountDownLatch latch(workerCount);

  auto woker = [&](int id) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000 * id));
    latch.countDown();
    printLock.lock();
    std::cout << "worker " << id << " finished task" << std::endl;
    printLock.unlock();
  };

  std::vector<std::thread> threads;
  for (int i = 0; i < workerCount; ++i) {
    threads.emplace_back(woker, i);
  }
  printLock.lock();
  std::cout << "Main thread waiting for workers" << std::endl;
  printLock.unlock();
  latch.wait();
  printLock.lock();
  std::cout << "All workers finished tasks\n";
  printLock.unlock();

  for (auto& t : threads) {
    t.join();
  }
}

int count = 1;                      // 共享计数器
bool flag = false;
std::condition_variable cv;          // 条件变量
std::mutex dummy_mutex;             // 互斥量
int try_count = 0;
void unsafeWait() {
  while (count > 0) {
    std::unique_lock<std::mutex> lock(dummy_mutex);
    flag = true;
    cv.wait(lock);
  }
  std::string error = "unsafeWait failed: " + std::to_string(try_count) + "\n";
  if (!flag) {
    std::cout << error;
    assert(flag && "Error");
  }
}

void unsafeCountDown() {
  --count;
  cv.notify_one();
}

int testUnsafe() {
  std::thread t1(unsafeWait);
  std::thread t2(unsafeCountDown);

  t1.join();
  t2.join();
  return 0;
}

int main() {
  testCountDownLatch();
  // for (int i = 0; i < 1000; i++) {
  //   count = 1;
  //   flag = false;
  //   try_count++;
  //   testUnsafe();
  // }
  return 0;
}


