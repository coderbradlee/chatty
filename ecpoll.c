
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include "ecpoll.h"


static socklen_t addr_len = sizeof(struct sockaddr_in);

int
set_non_blocking(int fd)
{
    int flags;
    flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
        return -1;
    flags |= O_NONBLOCK;
    return fcntl(fd, F_SETFL, flags);
}


struct ecpoll_inst*
ecpoll_inst_new(int event_capacity,
        ecpoll_inst_startup startup_handler,
        ecpoll_inst_handler server_handler,
        ecpoll_inst_handler client_handler,
        ecpoll_inst_cleanup cleanup_handler)
{
    struct ecpoll_inst* inst = 
        (struct ecpoll_inst*)malloc(sizeof(struct ecpoll_inst));
    if (!inst)
        goto ecpoll_inst_new_error1;
    bzero(inst, sizeof(struct ecpoll_inst));
    inst->events = (struct epoll_event*)malloc(
            sizeof(struct epoll_event) * event_capacity);
    if (!inst->events)
        goto ecpoll_inst_new_error2;
    bzero(inst->events,  sizeof(struct epoll_event) * event_capacity);

    inst->startup_handler = startup_handler;
    inst->event_capacity = event_capacity;
    inst->server_handler = server_handler;
    inst->client_handler = client_handler;
    inst->cleanup_handler = cleanup_handler;
    return inst;
ecpoll_inst_new_error2:
    free(inst);
ecpoll_inst_new_error1:
    return NULL;
}


void
ecpoll_inst_free(struct ecpoll_inst* inst)
{
    if (inst->events) {
        free(inst->events);
    }
    free(inst);
}


int
ecpoll_inst_prepare(struct ecpoll_inst* inst)
{
    int yes = 1;

    bzero((char *)&(inst->server_addr), sizeof(inst->server_addr));
    inst->server_addr.sin_family = AF_INET;
    inst->server_addr.sin_addr.s_addr = INADDR_ANY;
    inst->server_addr.sin_port = htons(inst->server_port);

    inst->server_fd = socket(AF_INET, SOCK_STREAM, 0);

    int rs = 0;

    rs |= set_non_blocking(inst->server_fd);
    rs |= setsockopt(inst->server_fd, SOL_SOCKET,
           SO_REUSEADDR, &yes, sizeof(int));
    rs |= bind(inst->server_fd,
           (struct sockaddr *)&(inst->server_addr),
           addr_len);
    if (rs != 0)
        goto ecpoll_inst_prepare_error1;

    inst->epoll_fd = epoll_create(inst->event_capacity);
    if (inst->epoll_fd == -1)
        goto ecpoll_inst_prepare_error1;

    return 0;
ecpoll_inst_prepare_error1:
    perror(strerror(errno));
    return -1;
}


struct ecpoll_keeper*
ecpoll_keeper_new(int inst_capacity)
{
    struct ecpoll_keeper* keeper = 
        (struct ecpoll_keeper*)malloc(sizeof(struct ecpoll_keeper));
    if (!keeper) goto ecpoll_keeper_new_err1;
    keeper->insts = (struct ecpoll_inst**)
        calloc(sizeof(struct ecpoll_inst*), inst_capacity);
    if (!keeper->insts) goto ecpoll_keeper_new_err2;
    keeper->threads = (pthread_t*)malloc(sizeof(pthread_t) * inst_capacity);
    if (!keeper->threads) goto ecpoll_keeper_new_err3;

    bzero(keeper->insts, sizeof(struct ecpoll_inst*) * inst_capacity);
    keeper->inst_capacity = inst_capacity;
    return keeper;
ecpoll_keeper_new_err3:
    free(keeper->insts);
ecpoll_keeper_new_err2:
    free(keeper);
ecpoll_keeper_new_err1:
    return NULL;
}


void
ecpoll_keeper_free(struct ecpoll_keeper *keeper)
{
    if (keeper) {
        int i = 0;
        for (; i < keeper->inst_capacity; i++) {
            if (keeper->insts[i]) {
                ecpoll_inst_free(keeper->insts[i]);
            }
        }
        free(keeper->insts);
        free(keeper->threads);
        free(keeper);
    }
}


int
ecpoll_keeper_register_instance(
        struct ecpoll_keeper* keeper,
        struct ecpoll_inst* inst)
{
    int i = 0;
    for (; i < keeper->inst_capacity; i++) {
        if (!keeper->insts[i]) {
            keeper->insts[i] = inst;
            return 0;
        }
    }
    return -1;
}


int
ecpoll_keeper_prepare(struct ecpoll_keeper* keeper)
{
    int i = 0;
    for (; i < keeper->inst_capacity; i++) {
        if (keeper->insts[i] &&
            ecpoll_inst_prepare(keeper->insts[i]) == -1)
            return -1;
    }
    return 0;
}


void*
ecpoll_inst_start(void* inst_void_addr)
{
    struct ecpoll_inst* inst = (struct ecpoll_inst*) inst_void_addr;

    printf("start %lu\n", pthread_self());

    if (inst->startup_handler) {
        inst->startup_handler(inst);
    }

    pthread_cleanup_push(inst->cleanup_handler, inst->data);

    listen(inst->server_fd, SOMAXCONN);

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = inst->server_fd;
    epoll_ctl(inst->epoll_fd, EPOLL_CTL_ADD, inst->server_fd, &ev);

    int client_fd, event_num, i;
    struct sockaddr_in client_addr;
    for (;;) {
        event_num = epoll_wait(inst->epoll_fd, inst->events,
                          inst->event_capacity, -1);
        if (event_num == -1)
            goto ecpoll_inst_start_error0;
        for (i = 0; i < event_num; ++i) {
            pthread_testcancel();
            ev = inst->events[i];
            if (ev.data.fd == inst->server_fd) {
                while (1) {
                    client_fd = accept(inst->server_fd,
                            (struct sockaddr *)&client_addr,
                            &addr_len);
                    if (client_fd == -1) {
                        if (errno == EAGAIN || errno == EWOULDBLOCK) {
                            break;
                        } else {
                            pthread_cancel(pthread_self());
                        }
                    } else {
                        if (inst->server_handler &&
                            inst->server_handler(ev, inst)) {
                            pthread_cancel(pthread_self());
                        }
                        set_non_blocking(client_fd);
                        ev.events = EPOLLOUT | EPOLLIN | EPOLLET;
                        ev.data.fd = client_fd;
                        epoll_ctl(inst->epoll_fd, EPOLL_CTL_ADD, client_fd, &ev);
                    }
                }
            } else {
                if (inst->client_handler && inst->client_handler(ev, inst)) {
                    pthread_cancel(pthread_self());
                }
            }
        }
    }

    pthread_cleanup_pop(0);
    pthread_exit(NULL);

ecpoll_inst_start_error0:
    perror(strerror(errno));
    pthread_exit(NULL);
}


void
ecpoll_keeper_start(struct ecpoll_keeper* keeper)
{
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CANCEL_ENABLE);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    int i = 0;
    for (; i < keeper->inst_capacity; i++) {
        if (keeper->insts[i]) {
            pthread_create(&keeper->threads[i], &attr,
                    ecpoll_inst_start, (void*)(keeper->insts[i]));
        }
    }

    pthread_attr_destroy(&attr);
}


void
ecpoll_keeper_stop(struct ecpoll_keeper* keeper)
{ 
    int i = 0;
    for (;i < keeper->inst_capacity; i++) {
        if (keeper->insts[i]) {
            pthread_cancel(keeper->threads[i]);
            pthread_join(keeper->threads[i], NULL);
            close(keeper->insts[i]->epoll_fd);
            close(keeper->insts[i]->server_fd);
        }
    }
}
