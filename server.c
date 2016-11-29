#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include "queue.h"
#include "server.h"

struct connArray{
    int fd;
    char recvBuff[1025];
    SLIST_ENTRY(connArray) socket_fds;
};

int max_fd = 0;
SLIST_HEAD(slisthead, connArray) socket_fds;

int runServer(int port)
{

    int listenfd = 0, connfd = 0;
    int n = 0, i = 0;
    struct sockaddr_in serv_addr;

    time_t ticks;
    fd_set readfds;
    struct timeval tv;
    int ret = 0;
    struct connArray *socketfd, *prev;

    SLIST_INIT(&socket_fds);

    FD_ZERO(&readfds);

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    printf("listen to port: %d\n", port);

    listen(listenfd, 128);

    FD_SET(listenfd, &readfds);
    max_fd = listenfd;
    printf("max_fd: %d\n", max_fd);

    while(1)
    {

        tv.tv_sec = 2;
        tv.tv_usec = 500000;

        ret = select(max_fd + 1, &readfds, NULL, NULL, &tv);
        if (ret > 0) {
            if(FD_ISSET(listenfd, &readfds)) {
                connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);

                printf("accept connection: %d\n", connfd);

                FD_SET(connfd, &readfds);
                if (connfd > max_fd) {
                    max_fd = connfd;
                }

                socketfd = (struct connArray *)malloc(sizeof(struct connArray));

                if (SLIST_EMPTY(&socket_fds)) {
                    socketfd->fd = connfd;
                    SLIST_INSERT_HEAD(&socket_fds, socketfd, socket_fds);
                } else {
                    SLIST_FOREACH(socketfd, &socket_fds, socket_fds) {
                        prev = socketfd;
                        ++i;
                    }
                    socketfd->fd = connfd;
                    SLIST_INSERT_AFTER(prev, socketfd, socket_fds);
                }
                FD_SET(listenfd, &readfds);
            }
            SLIST_FOREACH(socketfd, &socket_fds, socket_fds) {
                if(FD_ISSET(socketfd->fd, &readfds)) {
                    while ( (n = read(socketfd->fd, socketfd->recvBuff, sizeof(socketfd->recvBuff)-1)) > 0 )
                    {
                        socketfd->recvBuff[n] = 0;
                        printf("get: %s\n", socketfd->recvBuff);
                        if (strncmp(socketfd->recvBuff, "exit", sizeof("exit")) == 0) {
                            printf("exit: %d\n", socketfd->fd);
                            close(socketfd->fd);
                            FD_CLR(socketfd->fd, &readfds);
                            SLIST_REMOVE(&socket_fds, socketfd, connArray, socket_fds);
                            free(socketfd);
                        }
                    }
                }
            }
        }
    }

    return 0;
}
