

#ifndef ECPOLL_H
#define ECPOLL_H


#include <sys/epoll.h>
#include <stdint.h>
#include <netinet/in.h>
#include <pthread.h>

typedef int (*ecpoll_inst_handler)(struct epoll_event ev,
                                   struct ecpoll_inst* inst);
typedef void (*ecpoll_inst_startup)(struct ecpoll_inst* inst);
typedef void (*ecpoll_inst_cleanup)(void* data);


struct ecpoll_inst {
    uint16_t server_port;
    int server_fd;
    int epoll_fd;
    int event_capacity;
    struct sockaddr_in server_addr;
    struct epoll_event* events;
    ecpoll_inst_startup startup_handler;
    ecpoll_inst_handler server_handler;
    ecpoll_inst_handler client_handler;
    ecpoll_inst_cleanup cleanup_handler;
    void* data;
};


struct ecpoll_keeper{
    struct ecpoll_inst** insts;
    pthread_t* threads;
    int inst_capacity;
};


struct ecpoll_inst*
ecpoll_inst_new(int event_capacity,
                ecpoll_inst_startup startup_handler,
                ecpoll_inst_handler server_handler,
                ecpoll_inst_handler client_handler,
                ecpoll_inst_cleanup cleanup_handler);
void ecpoll_inst_free(struct ecpoll_inst*inst);


struct ecpoll_keeper* ecpoll_keeper_new(int inst_capacity);
void ecpoll_keeper_free(struct ecpoll_keeper *keeper);
int ecpoll_keeper_register_instance(
        struct ecpoll_keeper* keeper,
        struct ecpoll_inst* inst);
int ecpoll_keeper_prepare(struct ecpoll_keeper* keeper);
void ecpoll_keeper_start(struct ecpoll_keeper* keeper);
void ecpoll_keeper_stop(struct ecpoll_keeper* keeper);


int set_non_blocking(int fd);
#endif
