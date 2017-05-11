#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <strings.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <signal.h>
#include <iostream>
#include <sstream>
#include <evutil.h>
#include "server.h"

#define SERVER_HOST "127.0.0.1"
#define SERVER_PORT 5000
#define MAX_QUERY_SIZE 4096
#define MAX_THREAD_COUNT 4

int main(int argc, char* argv[]) {
    Server epoll_server((char *) SERVER_HOST, SERVER_PORT, MAX_THREAD_COUNT, MAX_QUERY_SIZE);
    epoll_server.run();
    return 0;
}
