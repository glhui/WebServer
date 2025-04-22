#pragma once

#include <functional>
#include <string>
#include <vector>
#include "CountDownLatch.h"
#include "LogStream.h"
#include "MutexLock.h"
#include "Thread.h"
#include "noncopyable.h"

class AsyncLogging : noncopyable {
public:
    AsyncLogging(const std::string basename, int flushInterval = 2);
    ~AsyncLogging(){
        if (running_) {
            stop();
        }
    };

    void append(const char* logline, int len);

    void start() {
        running_ = true;
        thread_.start();
        latch_.wait();
    }

    void stop() {
        running_ = false;
        cond_.notify();
        thread_.join();
    };
private:
    void threadFunc();
    using Buffer = FixedBuffer<kLargeBuffer>;
    using BufferPtr = std::shared_ptr<Buffer>;
    using BufferVector = std::vector<BufferPtr>; // 拷贝的时候性能更高

    const int flushInterval_;
    bool running_;
    std::string basename_;
    Thread thread_;
    MutexLock mutex_;
    Condition cond_;
    BufferPtr currentBuffer_;
    BufferPtr nextBuffer_;
    BufferVector buffers_;
    CountDownLatch latch_;
};