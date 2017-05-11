#include <fcntl.h>
#include <cstdio>
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
#include "epoll.h"
#include "client.h"

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

Server::Server(char *host, int port, int thread_count, int query_client) {
    const int BACK_LOG = 100;
    _select_id=0;
    _epollEngines = NULL;
    _threads = NULL;
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
    _thread_count =thread_count;
    _threads = new pthread_t[_thread_count];
    _epollEngines = new EpollEngine[_thread_count];
    for (int i = 0; i < _thread_count; ++i) {
        _epollEngines[i] = EpollEngine(query_client);
    }
    int status=0;
    void * arg = nullptr;
    for (int i = 0; i < _thread_count; i++) {
        status = pthread_create(&_threads[i], NULL, _epollEngines[i].run, arg);
        if (status != 0) {
            printf("main error: can't create thread, status = %d\n", status);
            exit(1);
        }
    }
}

Server::~Server() {
    close(_listener);
    if(_epollEngines!=NULL) delete(_epollEngines);
    if(_threads!=NULL) delete (_threads);
}

void Server::run() {
    unsigned int clientSize = sizeof(sockaddr_in);
    while (true) {
        sockaddr_in clientAddr;
        int clientDescriptor = accept(_listener, (sockaddr*)&clientAddr, &clientSize);
        if (clientDescriptor<0) continue;
        setNonBlocking(clientDescriptor);
        select(clientDescriptor);
    }
}

void Server::select(int clientDescriptor) {
    EpollEngine *engine = &_epollEngines[_select_id++];
    engine->addClient(new Client(clientDescriptor,engine));
    _select_id%=_thread_count;
}