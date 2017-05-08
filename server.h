//
// Created by alexandr on 08.05.17.
//

#ifndef WEBSERVER_SERVER_H
#define WEBSERVER_SERVER_H


class Server {
public:
    Server(char* host, int port, int thread_count);
    ~Server();

private:
    int _thread_count;
    int _listener;
};


#endif //WEBSERVER_SERVER_H
