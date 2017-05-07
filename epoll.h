#ifndef WEBSERVER_EPOLL_H
#define WEBSERVER_EPOLL_H

#include "classes.h"

#define EPOLL_QUEUE_LEN 65536

class EpollEngine {
public:
    EpollEngine(int query_client, int listener);
    ~EpollEngine();
    void run();
    void addClient(Client *client);
private:
    int _epoll_fd;
    int _listener;

};


#endif //WEBSERVER_EPOLL_H
