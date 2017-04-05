//
// Created by alexandr on 05.04.17.
//

#ifndef WEBSERVER_CLIENT_H
#define WEBSERVER_CLIENT_H


#include <cstdint>
#include <zconf.h>
#include <string>

using namespace std;

enum class client_state_t: uint8_t
{
    WANT_READ,
    WANT_WRITE
};


/*
    Client - is just an sd and state
*/
struct Client
{
    explicit Client(int sd)              : _sd(sd), _state(client_state_t::WANT_READ) {}
    Client(int sd, client_state_t state) : _sd(sd), _state(state) {}
    virtual ~Client() { if (_sd > 0) ::close (_sd); }

    virtual void onRead(string &str) {}
    virtual void onWrite()                      {}
    virtual void onDead()                       {}

    int _sd;
    client_state_t _state;
};


#endif //WEBSERVER_CLIENT_H
