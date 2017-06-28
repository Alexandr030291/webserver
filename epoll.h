#ifndef WEBSERVER_EPOLL_H
#define WEBSERVER_EPOLL_H

#include <atomic>
#include "client.h"

class EpollEngine {
public:
    EpollEngine() = default;
    EpollEngine(int query_client,int timeout);
    ~EpollEngine();
    void run();
    void addClient(int clientDescriptor);
    static std::atomic<bool> g_Stop;
private:
    int _epoll_fd;
    int _max_epoll_event;
    int _max_time_out;
};


#endif //WEBSERVER_EPOLL_H
