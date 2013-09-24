#include <stdio.h>
#include <stdlib.h>
#include "global.h"
#include "chattyio.h"

int main(int argc, const char *argv[])
{
    init_global_variables();
    init_chatty_io();


    /*for (;;) {*/
    /*    nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);*/
    /*    if (nfds == -1) {*/
    /*        perror("epoll_pwait");*/
    /*        exit(EXIT_FAILURE);*/
    /*    }*/

    /*    for (n = 0; n < nfds; ++n) {*/
    /*        if (events[n].data.fd == listen_sock) {*/
    /*            conn_sock = accept(listen_sock,*/
    /*                    (struct sockaddr *) &local, &addrlen);*/
    /*            if (conn_sock == -1) {*/
    /*                perror("accept");*/
    /*                exit(EXIT_FAILURE);*/
    /*            }*/
    /*            setnonblocking(conn_sock);*/
    /*            ev.events = EPOLLIN | EPOLLET;*/
    /*            ev.data.fd = conn_sock;*/
    /*            if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock,*/
    /*                        &ev) == -1) {*/
    /*                perror("epoll_ctl: conn_sock");*/
    /*                exit(EXIT_FAILURE);*/
    /*            }*/
    /*        } else {*/
    /*            do_use_fd(events[n].data.fd);*/
    /*        }*/
    /*    }*/
    /*}*/


    printf("hello\n");
    printf("%d\n", ws_server_port);
    return 0;
}
