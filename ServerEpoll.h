#ifndef WEBSERVER_SERVER_H
#define WEBSERVER_SERVER_H

#include <vector>
#include "UrlKey.h"
#include "FunctionRequest.h"
#include "Client.h"

typedef char* HOST;
typedef uint16_t PORT;

namespace engine{
    enum class op_error_t: uint8_t    {
        OP_EOF,
        OP_HUP
    };

    // Абстрактные event'ы, за которыми должен уметь следить Engine
    enum class event_t: uint8_t    {
        EV_NONE,
        EV_READ,
        EV_WRITE
    };

    struct OperationState{
        size_t transferred;
        op_error_t error;
    };
}

void sig_handler(int signum){
    ServerEpoll::g_Stop = true;
    //Config::impl()->destroy();
}

class ServerEpoll {
public:
    static std::atomic<bool> g_Stop;
    ServerEpoll(HOST host = (HOST) DEFAULT_HOST, PORT port = DEFAULT_PORT);
    ~ServerEpoll();
    void run();
    bool addToEventLoop(Client *c, engine::event_t events);
    void changeEvents(Client *c, engine::event_t events);
    bool isMyHost(const std::string &host) const;
protected:
    int listener() const { return m_Listener; }
private:
    int epoll_fd;
    int listenSocket(HOST, PORT, uint32_t listen_queue_size);
    int m_Listener;
    std::vector<uint32_t> m_LocalIPs;
    void event_loop();
};

#endif //WEBSERVER_SERVER_H
