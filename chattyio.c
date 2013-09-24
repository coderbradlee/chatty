
#include "chattyio.h"


    void
init_chatty_io(void)
{
    int listen_sock, yes = 1, epoll_fd;
    struct sockaddr_in serv_addr;
    struct epoll_event ev;

    // sock, bind and listen
    listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    GOTO_IF_TRUE(listen_sock < 0, init_chatty_io_fatal);
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(ws_server_port);
    GOTO_IF_TRUE(setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) < 0, init_chatty_io_fatal);
    GOTO_IF_TRUE(bind(listen_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0, init_chatty_io_fatal);
    listen(listen_sock, SOMAXCONN);

    // setup epoll event
    epoll_fd = epoll_create(10);
    GOTO_IF_TRUE(epoll_fd == -1, init_chatty_io_fatal);

    ev.events = EPOLLIN;
    ev.data.fd = listen_sock;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_sock, &ev) == -1) {
        perror("epoll_ctl: listen_sock");
        exit(EXIT_FAILURE);
    }

    


    return;
init_chatty_io_fatal:
    perror(strerror(errno));
    free_global_variables();
    exit(EXIT_FAILURE);
}
