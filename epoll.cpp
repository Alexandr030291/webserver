#include <iostream>
#include <sys/epoll.h>

#include "epoll.h"

std::atomic<bool> EpollEngine::g_Stop(false);



EpollEngine::EpollEngine(int query_client,int timeout) {
    _epoll_fd = epoll_create(query_client);
    _max_epoll_event = query_client;
    _max_time_out = timeout;
}

EpollEngine::~EpollEngine() {}

void EpollEngine::run() {
    Client *client;
    struct epoll_event events[_max_epoll_event];
    while (!EpollEngine::g_Stop){
        int nfds = epoll_wait(_epoll_fd, events, _max_epoll_event, _max_time_out);
        for (int i = 0; i < nfds; i++) {
            client = (Client*)(events[i].data.ptr);
     //       while (client!= nullptr) {
                if ((events[i].events & EPOLLHUP) || (events[i].events & EPOLLERR)) {
                    client->onDead();
                }

                if (events[i].events & EPOLLIN) {
                    if (client->isStatus() == client_state_t::WANT_READ)
                        client->onRead();
                }

                if (events[i].events & EPOLLOUT) {
                    if (client->isStatus() == client_state_t::WANT_WRITE)
                        client->onWrite();
                }

                if (client->isStatus() == client_state_t::WANT_CLOSE) {
                    delete client;
               }
        //    }
        }
    }
}

void EpollEngine::addClient(int clientDescriptor) {
    void *client= new Client(clientDescriptor);
    epoll_event ev;
    ev.events = EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLOUT;
    ev.data.ptr = client;
    epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, clientDescriptor, &ev);
}

