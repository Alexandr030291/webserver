#include <fcntl.h>
#include <cstdio>
#include <zconf.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
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

int setNonBlocking(int sock) {
    int opts;

    opts = fcntl(sock, F_GETFL);
    if (opts < 0) {
        perror("fcntl(F_GETFL)");
        return -1;
    }
    opts = (opts | O_NONBLOCK);
    if (fcntl(sock, F_SETFL, opts) < 0) {
        perror("fcntl(F_SETFL)");
        return -1;
    }

    return 0;
}

Server::Server(char *host, int port, int thread_count) {
    const int BACK_LOG = 100;

    _listener = socket(AF_INET, SOCK_STREAM, 0);
    if (_listener < 0) {
        perror("Socket creation");
        exit(1);
    }
    setNonBlocking(_listener);
    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(host);
    servaddr.sin_port = htons((uint16_t) port);

    if (bind(_listener, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        perror("Socket bind");
        exit(1);
    }

    if (listen(_listener, BACK_LOG) < 0) {
        perror("Socket listen");
        exit(1);
    }
}

Server::~Server() {
    close(_listener);
}
