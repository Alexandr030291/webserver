#ifndef WEBSERVER_PATHS_H
#define WEBSERVER_PATHS_H

#include <sys/epoll.h>
#include <iostream>
#include <map>
#include <unistd.h> //close()
#include <cstring>
#include <atomic>
#include <cstdint>
#include <stdint-gcc.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>

using namespace std;

#define DEFAULT_PORT 5000
#define DEFAULT_HOST "127.0.0.1"
#define MAX_EPOLL_EVENTS 32768
#define BACK_LOG 100
#define EPOLL_RUN_TIMEOUT 30000

enum METHOD{
    GET,
    HEARD
};

enum FUNCTION_TYPE{
    HTML,
    FILE_DOWNLOAD,
    LOGIC
};

typedef string PATH;
#endif //WEBSERVER_PATHS_H
