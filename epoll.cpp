#include <iostream>
#include <sys/epoll.h>
#include <bits/fcntl-linux.h>
#include <fcntl.h>

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

EpollEngine::EpollEngine(int query_client,int listener): _listener(listener) {
    _epoll_fd = epoll_create(query_client);
}

EpollEngine::~EpollEngine() {}

void EpollEngine::run() {

}

void EpollEngine::addClient(Client *client) {
    epoll_event ev;
    ev.events = EPOLLIN | EPOLLERR | EPOLLHUP;
    ev.data.ptr = (void*)client;
    int res = epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, client->getDescription(), &ev);
}

