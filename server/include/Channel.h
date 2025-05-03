#pragma once
#include <sys/epoll.h>
#include <sys/epoll.h>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include "Timer.h"

class EventLoop;
class HttpData;

class Channel {
public:
    Channel(EventLoop* loop);
    Channel(EventLoop* loop, int fd);
    ~Channel();
    int getFd();
    void setFd(int fd);

    std::shared_ptr<HttpData> getHolder() {
        std::shared_ptr<HttpData> ret(holder_.lock());
        return ret;
    }

    __uint32_t &getEvents() { return events_; }

    bool EqualAndUpdateLastEvents() {
        bool ret = (lastEvents_ == events_);
        lastEvents_ = events_;
        return ret;
      }
    
      __uint32_t getLastEvents() { return lastEvents_; }
private:
    typedef std::function<void()> CallBack;
    EventLoop *loop_;
    int fd_;
    __uint32_t events_;
    __uint32_t revents_;
    __uint32_t lastEvents_;

    std::weak_ptr<HttpData> holder_;
};

typedef std::shared_ptr<Channel> SP_Channel;