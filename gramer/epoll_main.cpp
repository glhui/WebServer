#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <arpa/inet.h>

#define MAX_EVENTS 1024
#define READ_BUFFER 4096
#define SERVER_PORT 8080

int set_non_blocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl");
        return -1;
    }
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int main() {
    int listen_fd, conn_fd, epoll_fd;
    struct sockaddr_in server_addr;
    struct epoll_event ev, events[MAX_EVENTS];

    // Create socket
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("setsockopt");
        close(listen_fd);
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);
    if (bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        close(listen_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(listen_fd, SOMAXCONN) < 0) {
        perror("listen");
        close(listen_fd);
        exit(EXIT_FAILURE);
    }

    // Create epoll instance
    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create1");
        close(listen_fd);
        exit(EXIT_FAILURE);
    }
    if (set_non_blocking(listen_fd) == -1) {
        close(listen_fd);
        close(epoll_fd);
        exit(EXIT_FAILURE);
    }
    ev.events = EPOLLIN;
    ev.data.fd = listen_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &ev) == -1) {
        perror("epoll_ctl: listen_fd");
        close(listen_fd);
        close(epoll_fd);
        exit(EXIT_FAILURE);
    }
    printf("Server listening on port %d\n", SERVER_PORT);

    while (1) {
        int n = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (n < 0) {
            perror("epoll_wait"); break;
        }
        for (int i = 0; i < n; i++) {
            int fd = events[i].data.fd;
            // 新连接
            if (fd == listen_fd) {
                struct sockaddr_in cli_addr;
                socklen_t addrlen = sizeof(cli_addr);
                conn_fd = accept(listen_fd, (struct sockaddr*)&cli_addr, &addrlen);
                if (conn_fd < 0) {
                    perror("accept"); continue;
                }
                set_non_blocking(conn_fd);
                ev.events = EPOLLIN | EPOLLET; // 边缘触发
                ev.data.fd = conn_fd;
                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, conn_fd, &ev);
                printf("New client: %s:%d, fd=%d\n",
                       inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port), conn_fd);
            }
            // 可读事件
            else if (events[i].events & EPOLLIN) {
                char buf[READ_BUFFER];
                while (1) {
                    ssize_t count = read(fd, buf, sizeof(buf));
                    if (count == -1) {
                        if (errno == EAGAIN) break; // 无更多数据
                        perror("read"); close(fd); break;
                    } else if (count == 0) {
                        // 客户端关闭
                        printf("Client fd=%d disconnected\n", fd);
                        close(fd);
                        break;
                    }
                    // 处理数据
                    printf("Received %s\n", buf);
                    // 回射数据
                    write(fd, buf, count);
                }
            }
        }
    }

    close(epoll_fd);
    close(listen_fd);
    return 0;
}