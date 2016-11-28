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
        {NULL, 0, NULL, 0}
    };

    int option_index = 0;
    int c = 0;
    int mode = 0;

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
        default:
            abort();
        }
    }

    switch(mode) {
        case 0:
            printf("select mode client\n");
            runClient();
            break;
        case 1:
            printf("select mode server\n");
            runServer();
            break;
        default:
            break;
    }

    return 0;
}
