#include <unistd.h>
#include <sys/socket.h>
#include "client.h"

int MAX_REQUEST_LEN = 4096;

void Client::onRead() {
    char * _request_data= new char[MAX_REQUEST_LEN + 1];
    memset(_request_data, 0, (size_t) (MAX_REQUEST_LEN + 1));
    recv(_client_descriptor, _request_data, (size_t) MAX_REQUEST_LEN, 0);
    if (_p_request == NULL) {
        _p_request = new RequestData(_request_data);
    }
    else {
        _p_request->init(_request_data);
    }
    if (_p_request->isValid){
        _state=client_state_t::WANT_WRITE;
        if (_p_response == NULL) {
            _p_response = new ResponseData(_p_request);
        }
        else {
            _p_response ->getHTTPResponse();
        }
    } else{
        _state=client_state_t::WANT_CLOSE;
    }
    delete _request_data;
    client_state_t::WANT_CLOSE;
}

Client::~Client() {
    if (_p_request  != NULL) delete _p_request ;
    _p_request  = NULL;
    if (_p_response != NULL) delete _p_response;
    _p_response = NULL;

    if (_client_descriptor > 0)
        ::close(_client_descriptor);
    _client_descriptor = -1;
}

Client::Client(int client_descriptor): _client_descriptor(client_descriptor),  _state(client_state_t::WANT_READ){
    _p_request = NULL;
    _p_response = NULL;

}

void Client::onWrite() {
    int sended;
    bool flag;
    if (_p_response->countHeaderSendedBytes != _p_response->headerLen) {
        sended = (int) send(_client_descriptor, _p_response->header, (size_t) _p_response->headerLen, 0);
        _p_response->countHeaderSendedBytes += sended;
    }
    if (_p_response->dataLen > 0 && _p_response->dataLen != _p_response->countDataSendedBytes) {
        sended = (int) send(_client_descriptor, _p_response->data, (size_t) _p_response->dataLen, 0);
        _p_response->countDataSendedBytes += sended;
    }
    flag = (((_p_response->dataLen) == (_p_response->countDataSendedBytes))
             && ((_p_response->countHeaderSendedBytes) == (_p_response->headerLen)));
    if (flag&&_p_request->keepAlive){
        clear();
    } //else{
        _state=client_state_t::WANT_CLOSE;
  //  }
}

void Client::onDead() {
    _state=client_state_t::WANT_CLOSE;
}

int Client::getDescription() {
    return _client_descriptor;
}
client_state_t Client::isStatus() {
    return _state;
}

void Client::clear() {
    _p_response->clear();
    _p_request->clear();
}