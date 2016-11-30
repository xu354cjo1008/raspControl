#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include "server.h"
#include "client.h"

static void print_usage(void) {
    printf("Usage: socketTest options\n");
    printf("\t-h\t--help\tDisplay usage information.\n"
           "\t-v\t--version\tPrint version\n"
           "\t-m\t--mode\tselect mode <server/client>\n");
    exit(0);
}

int main(int argc, char **argv)
{
    static struct option long_options[] = {
        {"help", 0, NULL, 'h'},
        {"version", 0, NULL, 'v'},
        {"mode", 1, NULL, 'm'},
        {"addr", 1, NULL, 'a'},
        {"port", 1, NULL, 'p'},
        {NULL, 0, NULL, 0}
    };

    int option_index = 0;
    int c = 0;
    int mode = 0;
    char *addr = "127.0.0.1";
    int port = 80;
    int rc = 0;

    while((c = getopt_long(argc, argv, "hvm:", long_options, &option_index)) != -1) {
        switch(c) {
        case 'h':
            print_usage();
        case 'v':
            break;
        case 'm':
            if (strncmp("server", optarg, sizeof("server")) == 0) {
                printf("select mode server\n");
                mode = 1;
            } else {
                printf("select mode client\n");
                mode = 0;
            }
            break;
        case 'a':
            addr = optarg;
            break;
        case 'p':
            port = atoi(optarg);
            break;
        default:
            abort();
        }
    }

    switch(mode) {
        case 0:
            printf("enter client mode\n");
            rc = runClient(addr, port);
            break;
        case 1:
            printf("enter server mode\n");
            rc = runServer(port);
            break;
        default:
            break;
    }

    return rc;
}
