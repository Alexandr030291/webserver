#ifndef WEBSERVER_EPOLL_H
#define WEBSERVER_EPOLL_H

#include <atomic>
#include "classes.h"

#define EPOLL_QUEUE_LEN 65536

class EpollEngine {
public:
    EpollEngine() = default;
    EpollEngine(int query_client);
    ~EpollEngine();
    static void * run(void * args);
    void addClient(Client *client);
    static std::atomic<bool> g_Stop;

    int get_epoll_fd() const;

    int get_max_epoll_event() const;

private:
    int _epoll_fd;
    int _max_epoll_event;
};


#endif //WEBSERVER_EPOLL_H
