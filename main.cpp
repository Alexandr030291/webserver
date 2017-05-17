#include "server.h"

#define SERVER_HOST "127.0.0.1"
#define SERVER_PORT 5000
#define MAX_QUERY_SIZE 4096
#define MAX_THREAD_COUNT 4
#define RUN_TIMEOUT 10

int main(int argc, char* argv[]) {
    Server epoll_server((char *) SERVER_HOST, SERVER_PORT, MAX_THREAD_COUNT, MAX_QUERY_SIZE, RUN_TIMEOUT);
    epoll_server.run();
    return 0;
}
