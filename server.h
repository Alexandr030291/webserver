//
// Created by alexandr on 08.05.17.
//

#ifndef WEBSERVER_SERVER_H
#define WEBSERVER_SERVER_H

#include "classes.h"

class Server {
public:
    Server(char* host, int port, int thread_count, int query_client);
    ~Server();
    void run();
private:
    int _thread_count;
    int _listener;
    int _select_id;
    EpollEngine * _epollEngines;
    void select(int clientDescriptor);
    pthread_t *_threads;
};


#endif //WEBSERVER_SERVER_H
