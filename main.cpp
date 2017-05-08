#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <strings.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <signal.h>
#include <iostream>
#include <sstream>
#include <evutil.h>

#define SERVER_HOST "127.0.0.1"
#define SERVER_PORT 5000

namespace {

    int setnonblocking(int sock);

    void do_read(int fd);

    void do_write(int fd);

    void process_error(int fd);

}

int main(int argc, char* argv[]) {
    const int MAX_EPOLL_EVENTS = 100;
    const int BACK_LOG = 100;
    signal(SIGPIPE, SIG_IGN);
    int efd = epoll_create(MAX_EPOLL_EVENTS);

    int listenfd;
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) {
        perror("Socket creation");
        return -1;
    }

    // Неблокирующий режим
    setnonblocking(listenfd);

    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(SERVER_HOST);
    servaddr.sin_port = htons(SERVER_PORT);


    if (bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        perror("Socket bind");
        return -1;
    }

    if (listen(listenfd, BACK_LOG) < 0) {
        perror("Socket listen");
        return -1;
    }
    struct epoll_event listenev;
    listenev.events = EPOLLIN;
    listenev.data.fd = listenfd;
    if (epoll_ctl(efd, EPOLL_CTL_ADD, listenfd, &listenev) < 0) {
        perror("Epoll fd add");
        return -1;
    }

    socklen_t client;
    struct epoll_event events[MAX_EPOLL_EVENTS];
    struct epoll_event connev;
    struct sockaddr_in cliaddr;

    int events_cout = 1;

    bool flag =true;

    while(flag){
        int nfds = epoll_wait(efd, events, MAX_EPOLL_EVENTS, -1);

        for (int n = 0; n < nfds; ++n) {
            if (events[n].data.fd == listenfd) {
                client = sizeof(cliaddr);
                int connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &client);
                if (connfd < 0) {
                    perror("accept");
                    continue;
                }

                // Недостаточно места в массиве ожидания
                if (events_cout == MAX_EPOLL_EVENTS - 1) {
                    std::cout << "Event array is full" << std::endl;
                    close(connfd);
                    continue;
                }
                setnonblocking(connfd);
                connev.data.fd = connfd;
                connev.events = EPOLLIN | EPOLLOUT | EPOLLET | EPOLLRDHUP;
                if (!epoll_ctl(efd, EPOLL_CTL_ADD, connfd, &connev) < 0) {
                    perror("Epoll fd add");
                    close(connfd);
                    continue;
                }

                ++events_cout;
            }
            else {
                int fd = events[n].data.fd;// почему то игнорируется и всегда срабатывает (events[n].events & EPOLLOUT)
                if (events[n].events & EPOLLIN) {
                    do_read(fd);
                }

                if (events[n].events & EPOLLOUT) {
                    do_read(fd);
                    do_write(fd);
                }

                if (events[n].events & EPOLLRDHUP) {
                    process_error(fd);
                    flag=false;
                }
                epoll_ctl(efd, EPOLL_CTL_DEL, fd, &connev);
                --events_cout;
                close(fd);
            }
        }
    }
    close(listenfd);
    return 0;
}

namespace {

    int setnonblocking(int sock) {
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

        return 0;
    }

    void do_write(int fd) {
        std::cout << "write" << std::endl;
    }

    void do_read(int fd) {
        std::cout << "read" << std::endl;

    }
}