#pragma once

#include <windows.h>
#include <processthreadsapi.h>
#include <thread>
#include <functional>
#include <memory>
#include <string>
#include <iostream>

#include <CountDownLatch.h>
#include <noncopyable.h>

class Thread : noncopyable {
public:
    using ThreadFunc = std::function<void()>;

    explicit Thread(const ThreadFunc& func, const std::string& name = std::string());

    ~Thread();

    void start();

    int join();

    bool started() const { return started_; }
    DWORD tid() const { return tid_; }
    const std::string& name() const { return name_; }

    static unsigned __stdcall startThread(void* arg);

private:
    void setDefaultName();
    bool started_;
    bool joined_;
    HANDLE threadHandle_;      // Windows thread handle
    unsigned int tid_;                // Windows thread ID
    ThreadFunc func_;
    std::string name_;
    CountDownLatch latch_;
};

// void setThreadName(const std::string& name) {
//   // TODO(setting thread name need fix)
// }