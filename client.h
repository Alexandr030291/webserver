//
// Created by alexandr on 04.05.17.
//

#ifndef WEBSERVER_CLIENT_H
#define WEBSERVER_CLIENT_H

#include <cstdint>
#include <string.h>
#include <iostream>
#include "classes.h"
#include "http.h"

enum class client_state_t: uint8_t
{
    WANT_READ,
    WANT_WRITE,
    WANT_CLOSE
};

class Client {
public:
    Client(int sd, EpollEngine *ev);
    virtual ~Client();
    void onRead();
    void onWrite();
    void onDead();
    int getDescription();
    client_state_t isStatus();
    void nextStatus(client_state_t);
    void clear();
private:
    EpollEngine *_ev;
    int _client_descriptor;
    client_state_t _state;
    RequestData * _p_request;
    ResponseData * _p_response;
    char * _request_data;
};


#endif //WEBSERVER_CLIENT_H
