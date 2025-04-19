//
// Created by 19948 on 2025/4/15.
//

// std lib
#include <functional>
#include <iostream>
#include <string>
#include <thread>
#include <sstream>
#include <cassert>
#include <Windows.h>

// public interface
#include "CurrentThread.h"
#include "Thread.h"

namespace CurrentThread {
    thread_local int t_cachedTid = 0;
    thread_local char t_tidString[32];
    thread_local int t_tidStringLen = 6;
    thread_local const char* t_threadName = "unknown";
}

void CurrentThread::cacheTid() {
    if (CurrentThread::t_cachedTid == 0) {
        CurrentThread::t_cachedTid = ::GetCurrentThreadId();
        int len = snprintf(CurrentThread::t_tidString, sizeof(CurrentThread::t_tidString), "%d", CurrentThread::t_cachedTid);
        assert(len < sizeof(CurrentThread::t_tidString));
        CurrentThread::t_tidStringLen = len;
    }
}

struct ThreadData {
    using ThreadFunc = std::function<void()>;
    ThreadFunc func_;
    std::string name_;
    unsigned int* tid_;
    CountDownLatch* latch_;

    ThreadData(const ThreadFunc& func, const std::string& name, unsigned int* tid, CountDownLatch* latch)
        : func_(func), name_(name), tid_(tid), latch_(latch) {}

    void runInThread() {
        *tid_ = CurrentThread::tid();
        tid_ = nullptr;

        if (latch_) {
            latch_->countDown();
            latch_ = nullptr;
        }

        CurrentThread::t_threadName = name_.empty() ? "Thread" : name_.c_str();
        setThreadName(CurrentThread::t_threadName);

        func_();

        CurrentThread::t_threadName = "finished";
    }
};

unsigned __stdcall Thread::startThread(void* arg) {
    ThreadData* data = static_cast<ThreadData*>(arg);
    data->runInThread();
    delete data;
    return 0;
}

void* startThread(void* obj) {
    ThreadData* data = static_cast<ThreadData*>(obj);
    data->runInThread();
    delete data;
    return NULL;
}

Thread::Thread(const ThreadFunc& func, const std::string& n)
    : started_(false),
      joined_(false),
      threadHandle_(0),
      tid_(0),
      func_(func),
      name_(n),
      latch_(1) {
    setDefaultName();
}

Thread::~Thread() {
    if (started_ && !joined_ && threadHandle_) CloseHandle(threadHandle_);
}

void Thread::setDefaultName() {
    if (name_.empty()) {
        char buf[32];
        snprintf(buf, sizeof buf, "Thread");
        name_ = buf;
    }
}

void Thread::start() {
    assert(!started_);
    started_ = true;
    ThreadData* data = new ThreadData(func_, name_, &tid_, &latch_);
    threadHandle_ = (HANDLE)_beginthreadex(nullptr, 0, &Thread::startThread, data, 0, &tid_);
    if (threadHandle_ == 0) {
        started_ = false;
        delete data;
    } else {
        latch_.wait();
        assert(tid_ > 0);
    }
}

int Thread::join() {
    assert(started_);
    assert(!joined_);
    joined_ = true;
    WaitForSingleObject(threadHandle_, INFINITE);
    int retVal = 0;
    CloseHandle(threadHandle_);
    threadHandle_ = nullptr;
    return retVal;
}