
#include <csignal>
#include <netdb.h>
#include "ServerEpoll.h"

atomic<bool> ServerEpoll::g_Stop(false);

static bool accept_action(int epfd, int listener, ServerEpoll *ev){
    struct sockaddr_in client;
    memset(&client, 0, sizeof(client));
    socklen_t cli_len = sizeof(client);

    int cli_sd = accept(listener, (struct sockaddr*)&client, &cli_len);
    if (cli_sd == -1)    {
        return false;
    }


}

ServerEpoll::ServerEpoll(HOST host,PORT port) {
    //Обрабатываем сигнал ошибки работы с сокетом
    signal(SIGPIPE, sig_handler);

    m_Listener = listenSocket(host,port,BACK_LOG);
    if (m_Listener<0)
        throw "Error open socket";
}

ServerEpoll::~ServerEpoll() {
    close(m_Listener);
}

void ServerEpoll::event_loop() {
    epoll_fd = epoll_create(MAX_EPOLL_EVENTS);

    // Добавляем дескриптор в массив ожидания
    struct epoll_event listen_ev;
    listen_ev.events = EPOLLIN | EPOLLPRI | EPOLLET;
    listen_ev.data.fd = m_Listener;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, m_Listener, &listen_ev) < 0)
    {
        perror("Epoll fd add");
        return;
    }

/*
        socklen_t client;
        // Массив готовых дескрипторов

        struct epoll_event connev;
        struct sockaddr_in cliaddr;

        int events_cout = 1;
*/
    struct epoll_event *events = (struct epoll_event*)malloc(MAX_EPOLL_EVENTS * sizeof(struct epoll_event));

    while (!g_Stop) {
        vector<Client *> disconnected_clients;
        // Блокирование до готовности одно или нескольких дескрипторов
        int epoll_ret = epoll_wait(epoll_fd, events, MAX_EPOLL_EVENTS, -1);
        if (epoll_ret == 0) continue;
        if (epoll_ret == -1){
            if (g_Stop){
                break;
            }
            throw runtime_error(std::string("epoll: ") + strerror(errno));
        }

        for (int i = 0; i < epoll_ret; ++i){
            if (events[i].data.fd == listener()){
                if (!accept_action(epoll_fd, listener(), this)){
                    throw "FATAL. accept!";
                }
                continue;
            }
        }
    }

    free(events);
    close(epoll_fd);
}

void ServerEpoll::run() {
    event_loop();
}

int ServerEpoll::listenSocket(HOST host,PORT port, uint32_t listen_queue_size) {
    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock <= 0) {
        perror("Socket creation");
        return -1;
    }

    struct sockaddr_in serv_addr;

    {
        // Неблокирующий режим
        int opts;

        opts = fcntl(sock, F_GETFL);
        if (opts < 0) {
            perror("fcntl(F_GETFL)");
            return -1;
        }
        opts = (opts | O_NONBLOCK);
        if (fcntl(sock, F_SETFL, opts) < 0) {
            perror("fcntl(F_SETFL)");
            return -1;
        }
    }
    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = (in_addr_t) gethostbyname(host);
    servaddr.sin_port = htons(port);

    if (bind(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("Socket bind");
        return -1;
    }

    if (listen(sock, listen_queue_size) < 0) {
        perror("Socket listen");
        return -1;
    }
    return sock;
}