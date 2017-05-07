#include <iostream>
#include <sys/epoll.h>
#include <bits/fcntl-linux.h>
#include <fcntl.h>

#include "epoll.h"
#include "client.h"

std::atomic<bool> EpollEngine::g_Stop(false);

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
    _max_epoll_event = query_client;
}

EpollEngine::~EpollEngine() {}

void EpollEngine::run() {
    Client *client;
    struct epoll_event events[_max_epoll_event];
    while (!EpollEngine::g_Stop){
        int nfds = epoll_wait(_epoll_fd, events, _max_epoll_event, -1);
        for (int i = 0; i < nfds; ++i) {
            client = (Client*)(events[i].data.ptr,this);
            if(events[i].events & EPOLLHUP|EPOLLERR) {
                client->onDead();
            }

            if (events[i].events & EPOLLOUT) {
                if (client->isStatus() == client_state_t::WANT_WRITE)
                    client->onRead();
            }

            if (events[i].events & EPOLLRDHUP) {
                if (client->isStatus()==client_state_t::WANT_WRITE)
                    client->onWrite();
            }

            if (client->isStatus()==client_state_t::WANT_CLOSE){
                delete client;
                continue;
            }
        }
    }
}

void EpollEngine::addClient(Client *client) {
    epoll_event ev;
    ev.events = EPOLLIN | EPOLLERR | EPOLLHUP;
    ev.data.ptr = (void*)client;
    int res = epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, client->getDescription(), &ev);
}

