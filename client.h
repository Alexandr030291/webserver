//
// Created by alexandr on 04.05.17.
//

#ifndef WEBSERVER_CLIENT_H
#define WEBSERVER_CLIENT_H

#include <cstdint>
#include <zconf.h>
#include <string.h>
#include <iostream>
#include "classes.h"

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
    void onRead(const std::string &str);
    void onWrite();
    void onDead();
    int getDescription();
    client_state_t isStatus();
private:
    EpollEngine *_ev;
    int _sd;
    client_state_t _state;
};


#endif //WEBSERVER_CLIENT_H
