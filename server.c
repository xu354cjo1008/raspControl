#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

typedef struct {
    int *array;
    size_t used;
    size_t size;
} connArray;

void initConnArray(connArray *arr, size_t initSize)
{
    arr->array = (int *)malloc(initSize * sizeof(int));
    arr->used = 0;
    arr->size = initSize;
}

void connArrayPush(connArray *arr, int conn)
{
    if (arr->used == arr->size) {
        arr->size *= 2;
        arr->array = (int *)malloc(arr->size * sizeof(int));
    }
    arr->array[arr->used++] = conn;
}

void freeConnArray(connArray *arr)
{
    if (arr->array) {
        free(arr->array);
        arr->array = NULL;
    }
    arr->size = 0;
    arr->used = 0;
}

int runServer(int port)
{

    int listenfd = 0, connfd = 0;
    int n = 0;
    struct sockaddr_in serv_addr;

    char sendBuff[1025];
    char recvBuff[1025];
    time_t ticks;
    connArray conns;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    printf("listen to port: %d\n", port);

    listen(listenfd, 10);

    initConnArray(&conns, 10);

    while(1)
    {
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);

        printf("accept connection: %d\n", connfd);

        while ( (n = read(connfd, recvBuff, sizeof(recvBuff)-1)) > 0 )
        {
            recvBuff[n] = 0;
            printf("get: %s\n", recvBuff);
//            if(fputs(recvBuff, stdout) == EOF)
//            {
//                printf("\n Error : Fputs error\n");
//            }
        }

        close(connfd);
        sleep(1);
    }

    return 0;
}
