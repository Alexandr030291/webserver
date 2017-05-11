#include <iostream>
#include <sys/epoll.h>

#include "epoll.h"
#include "client.h"

std::atomic<bool> EpollEngine::g_Stop(false);


EpollEngine::EpollEngine(int query_client) {
    _epoll_fd = epoll_create(query_client);
    _max_epoll_event = query_client;
}

EpollEngine::~EpollEngine() {}

void * EpollEngine::run(void * args) {
    EpollEngine *engine = (EpollEngine *) &args;
    Client *client;
    struct epoll_event events[engine->get_max_epoll_event()];
    while (!EpollEngine::g_Stop){
        int nfds = epoll_wait(engine->get_epoll_fd(), events, engine->get_max_epoll_event(), -1);
        for (int i = 0; i < nfds; ++i) {
            client = (Client*)(events[i].data.ptr,engine);
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

int EpollEngine::get_epoll_fd() const {
    return _epoll_fd;
}

int EpollEngine::get_max_epoll_event() const {
    return _max_epoll_event;
}

