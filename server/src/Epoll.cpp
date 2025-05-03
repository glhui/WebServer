#include "Epoll.h"
#include <assert.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <deque>
#include <queue>
#include "Util.h"
#include "Logging.h"

#include <arpa/inet.h>
#include <iostream>

using namespace std;

const int EVENTSNUM = 4096;
const int EPOLLWAIT_TIME = 10000;

typedef shared_ptr<Channel> SP_Channel;

Epoll::Epoll() : 
    epollFd_(epoll_create1(EPOLL_CLOEXEC)), 
    events_(EVENTSNUM) {
    assert(epollFd_ > 0);
}

Epoll::~Epoll() {}

void Epoll::epoll_add(SP_Channel request, int timeout) {
    int fd = request->getFd();
    if (timeout > 0) {
        add_timer(request, timeout);
        fd2http_[fd] = request->getHolder();
    }
    struct epoll_event event;
    event.data.fd = fd;
    event.events = request->getEvents();

    request->EqualAndUpdateLastEvents();

    fd2chan_[fd] = request;
    if (epoll_ctl(epollFd_, EPOLL_CTL_ADD, fd, &event) < 0) {
        perror("epoll_add error");
        fd2chan_[fd].reset();
    }
}