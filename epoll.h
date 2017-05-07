#ifndef WEBSERVER_EPOLL_H
#define WEBSERVER_EPOLL_H

#include <atomic>
#include "classes.h"

#define EPOLL_QUEUE_LEN 65536

class EpollEngine {
public:
    EpollEngine(int query_client, int listener);
    ~EpollEngine();
    void run();
    void addClient(Client *client);
    static std::atomic<bool> g_Stop;
private:
    int _epoll_fd;
    int _listener;
    int _max_epoll_event;
};


#endif //WEBSERVER_EPOLL_H
