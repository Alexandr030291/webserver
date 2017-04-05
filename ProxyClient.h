#ifndef WEBSERVER_PROXYCLIENT_H
#define WEBSERVER_PROXYCLIENT_H


#include "Client.h"
#include "ServerEpoll.h"

class ProxyClient: public Client {
public:
    ProxyClient(int sd, ServerEpoll *ev);
    virtual ~ProxyClient();
private:

protected:
};


#endif //WEBSERVER_PROXYCLIENT_H
