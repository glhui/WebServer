// echo_client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP   "127.0.0.1"
#define SERVER_PORT 8080
#define BUFFER_SIZE 4096

int main() {
    int sockfds[10];
    struct sockaddr_in serv_addr;
    char send_buf[] = "Hello, epoll echo server!";
    char recv_buf[BUFFER_SIZE];
    ssize_t n;

    for (int i = 0; i < 10; i++) {
        if ((sockfds[i] = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            perror("socket");
            exit(EXIT_FAILURE);
        }
    }

    // 2. 填充服务器地址结构
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        perror("inet_pton");
        for (int i = 0; i < 10; i++) {
            close(sockfds[i]);
        }
        exit(EXIT_FAILURE);
    }

    // 3. 连接服务器
    for (int i = 0; i < 10; i++) {
        if (connect(sockfds[i], (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
            perror("connect");
            close(sockfds[i]);
            exit(EXIT_FAILURE);
        }
        printf("Connected to %s:%d\n", SERVER_IP, SERVER_PORT);
    }

    // 4. 发送数据
    for (int i = 0; i < 10; i++) {
        char send_buf[BUFFER_SIZE];
        snprintf(send_buf, sizeof(send_buf), "Hello from client %d", i);
        n = write(sockfds[i], send_buf, strlen(send_buf));
        if (n < 0) {
            perror("write");
            close(sockfds[i]);
            exit(EXIT_FAILURE);
        }
        printf("Sent: %s\n", send_buf);
    }

    // 5. 接收回显
    printf("Received: %s\n", recv_buf);
    for (int i = 0; i < 10; i++) {
        n = read(sockfds[i], recv_buf, sizeof(recv_buf)-1);
        if (n < 0) {
            perror("read");
            close(sockfds[i]);
            exit(EXIT_FAILURE);
        }
        recv_buf[n] = '\0';
        printf("Received: %s\n", recv_buf);
    }

    // 6. 关闭
    for (int i = 0; i < 10; i++) {
        close(sockfds[i]);
        printf("Closed socket %d\n", sockfds[i]);
    }
    return 0;
}
