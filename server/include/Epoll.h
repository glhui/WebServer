#pragma once
#include <sys/epoll.h>
#include <memory>
#include <unordered_map>
#include <vector>
#include "Channel.h"
#include "HttpData.h"
#include "Timer.h"

class Epoll {
public:
    Epoll();
    ~Epoll();
    void epoll_add(SP_Channel request, int timeout);
    void epoll_mod(SP_Channel request, int timeout);
    void epoll_del(SP_Channel request);

    std::vector<SP_Channel> poll();
    std::vector<SP_Channel> getEventsRequest(int events_num);
    void add_timer(SP_Channel request, int timeout);

    int getEpollFd() { return epollFd_; }
    void handleExpired();
private:
    static const int MAXFDS = 10000;
    int epollFd_;
    std::vector<epoll_event> events_;
    std::shared_ptr<Channel> fd2chan_[MAXFDS];
    std::shared_ptr<HttpData> fd2http_[MAXFDS];
    TimerManager timerManager_;
};