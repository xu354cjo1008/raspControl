#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include "queue.h"
#include "server.h"

struct connection{
    int fd;
    char recvBuff[1025];
    SLIST_ENTRY(connection) socket_fds;
};

struct connMgr{
    fd_set readfds;
    fd_set writefds;
    int max_fd;
    int listenfd;
    SLIST_HEAD(slisthead, connection) socket_fds;
};

static struct connMgr * newConnMgr(void)
{
    struct connMgr *c = (struct connMgr *)malloc(sizeof(struct connMgr));
    SLIST_INIT(&c->socket_fds);
    FD_ZERO(&c->readfds);
    FD_ZERO(&c->writefds);
    return c;
}

static int connAccept(struct connMgr *s)
{
    struct connection *socketfd, *prev;
    int i = 0;
    int connfd = accept(s->listenfd, (struct sockaddr*)NULL, NULL);

    if (connfd < 0) {
        perror("  accept() failed");
        return -1;
    }

    printf("accept connect fd: %d\n", connfd);

    FD_SET(connfd, &s->readfds);
    if (connfd > s->max_fd) {
        s->max_fd = connfd;
    }

    if (SLIST_EMPTY(&s->socket_fds)) {
        socketfd = (struct connection *)malloc(sizeof(struct connection));
        socketfd->fd = connfd;
        SLIST_INSERT_HEAD(&s->socket_fds, socketfd, socket_fds);
    } else {
        SLIST_FOREACH(socketfd, &s->socket_fds, socket_fds) {
            prev = socketfd;
            ++i;
        }
        socketfd = (struct connection *)malloc(sizeof(struct connection));
        socketfd->fd = connfd;
        SLIST_INSERT_AFTER(prev, socketfd, socket_fds);
    }

    return connfd;
}

static void connClose(struct connMgr *s, struct connection *socketfd)
{
    printf("exit: %d\n", socketfd->fd);
    SLIST_REMOVE(&s->socket_fds, socketfd, connection, socket_fds);
    FD_CLR(socketfd->fd, &s->readfds);
    close(socketfd->fd);
    free(socketfd->recvBuff);
    free(socketfd);
}

int runServer(int port)
{

    int flag = 1;
    struct sockaddr_in serv_addr, client_addr;

    time_t ticks;
    struct timeval tv;
    int rc = 0;
    struct connection *socketfd, *prev;

    struct connMgr *connMgr = newConnMgr();

    connMgr->listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);

    if (setsockopt(connMgr->listenfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int)) < 0) {
        perror("setsockopt()");
        return -1;
    }

//    int flags= fcntl(listenfd, F_GETFL, 0);
//    fcntl(listenfd, F_SETFL, flags | O_NONBLOCK);

    bind(connMgr->listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    printf("listenfd: %d, listen to port: %d\n", connMgr->listenfd, port);

    listen(connMgr->listenfd, 3);

    FD_SET(connMgr->listenfd, &connMgr->readfds);
    connMgr->max_fd = connMgr->listenfd;
    printf("max_fd: %d\n", connMgr->max_fd);

    while(1) {

        tv.tv_sec = 2;
        tv.tv_usec = 500000;

        rc = select(connMgr->max_fd + 1, &connMgr->readfds, NULL, NULL, &tv);
        if (rc > 0) {
            if(FD_ISSET(connMgr->listenfd, &connMgr->readfds)) {
                connAccept(connMgr);
            }
            SLIST_FOREACH(socketfd, &connMgr->socket_fds, socket_fds) {
                if(FD_ISSET(socketfd->fd, &connMgr->readfds)) {

                    rc = read(socketfd->fd, socketfd->recvBuff, sizeof(socketfd->recvBuff)-1);
                    if (rc <= 0) {
                        if (errno == EAGAIN) {
                            printf("again\n");
                            continue;
                        } else {
                            perror("  recv() failed");
                            return -1;
                        }
                    }

                    socketfd->recvBuff[rc] = 0;
                    printf("get: %s\n", socketfd->recvBuff);
                    if (strncmp(socketfd->recvBuff, "exit", sizeof("exit")) == 0) {
                        connClose(connMgr, socketfd);
                    }
                }
            }
        }
    }

    return 0;
}
