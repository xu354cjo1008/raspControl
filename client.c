#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

int runClient(char *address, int port)
{
    int sockfd = 0, n = 0;
    char str[100];
    char recvBuff[1024];
    char sendBuff[1024];
    struct sockaddr_in serv_addr;

    memset(recvBuff, '0',sizeof(recvBuff));
    memset(sendBuff, '0',sizeof(sendBuff));
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if(inet_pton(AF_INET, address, &serv_addr.sin_addr)<=0)
    {
        printf("\n inet_pton error occured\n");
        return 1;
    }

    if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0 )
    {
        printf("\n Error : Connect Failed \n");
        return 1;
    }

    printf("connect to remote: %s:%d\n", address, port);

    while(1) {
        printf( "Enter a value :" );
        scanf("%s", sendBuff);
        printf( "Entered : %s\n", sendBuff);
        write(sockfd, sendBuff, sizeof(sendBuff));
        if (strncmp(sendBuff, "exit", sizeof("exit")) == 0) {
            break;
        }
    }

    return 0;

}
