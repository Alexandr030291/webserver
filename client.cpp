//
// Created by alexandr on 04.05.17.
//

#include "client.h"

Client::~Client() {
    if (_sd > 0)
        ::close(_sd);
    _sd = -1;
}

void Client::onRead(const std::string &str) {

}

Client::Client(int sd, EpollEngine *ev): _sd(sd), _ev(ev), _state(client_state_t::WANT_READ){}

void Client::onWrite() {

}

void Client::onDead() {

}

int Client::getDescription() {
    return _sd;
}
 client_state_t Client::isStatus() {
    return _state;
}
