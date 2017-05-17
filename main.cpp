#include "server.h"

#define SERVER_HOST "127.0.0.1"
#define SERVER_PORT 5000
#define MAX_QUERY_SIZE 4096
#define MAX_THREAD_COUNT 4
#define RUN_TIMEOUT 10


void * controller(void *arg){
    bool stop = !(bool) arg;
    const int max_buffer = 2048;
    const char STOP[] = "STOP";
    char buffer[max_buffer]= {'\0'};

    while(stop){
        scanf("%s",buffer);
        if (strcmp(buffer,STOP)==0){
            stop=!stop;
            EpollEngine::g_Stop = true;
        }
    }
    return nullptr;
}

int main(int argc, char* argv[]) {
    pthread_t command;
    pthread_create(&command, NULL, controller, false);
    Server epoll_server((char *) SERVER_HOST, SERVER_PORT, MAX_THREAD_COUNT, MAX_QUERY_SIZE, RUN_TIMEOUT);
    epoll_server.run();
    pthread_join(command, NULL);
    return 0;
}
