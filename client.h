//
// Created by alexandr on 04.05.17.
//

#ifndef WEBSERVER_CLIENT_H
#define WEBSERVER_CLIENT_H

#include <cstdint>
#include <string.h>
#include <iostream>
#include <atomic>
#include "http.h"


enum class client_state_t: uint8_t{
    WANT_READ,
    WANT_WRITE,
    WANT_CLOSE
};

class Client {
public:
    Client(int client_descriptor);
    virtual ~Client();
    void onRead();
    void onWrite();
    void onDead();
    int getDescription();
    client_state_t isStatus();

    void clear();
private:
    int _client_descriptor;
    client_state_t _state;
    RequestData * _p_request;
    ResponseData * _p_response;
 //   char * _request_data;
};


#endif //WEBSERVER_CLIENT_H