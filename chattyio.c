
#include "chattyio.h"

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

void
init_chatty_io(void)
{
    int listen_sock, yes = 1, epoll_fd, nfds, conn_sock, n, flags;
    socklen_t addr_len;
    struct sockaddr_in serv_addr, cli_addr;
    struct epoll_event ev;

    // sock, bind and listen
    listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    set_non_blocking(listen_sock);
    GOTO_IF(listen_sock == -1, init_chatty_io_fatal);
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(ws_server_port);
    GOTO_IF(setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1, init_chatty_io_fatal);
    GOTO_IF(bind(listen_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1, init_chatty_io_fatal);
    listen(listen_sock, SOMAXCONN);

    // setup and add listen_sock into epoll event loop
    epoll_fd = epoll_create(10);
    GOTO_IF(epoll_fd == -1, init_chatty_io_fatal);
    ev.events = EPOLLIN;
    ev.data.fd = listen_sock;
    GOTO_IF(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_sock, &ev) == -1, init_chatty_io_fatal);
    addr_len = sizeof(struct sockaddr_in);

    // main event loop
    for (;;) {
        nfds = epoll_wait(epoll_fd, events, max_events, -1);
        GOTO_IF(nfds == -1, init_chatty_io_fatal);
        for (n = 0; n < nfds; ++n) {
            if (events[n].data.fd == listen_sock) {
                while (1) {
                    // incomming new socket
                    conn_sock = accept(listen_sock, (struct sockaddr *)&cli_addr, &addr_len);
                    if (conn_sock == -1) {
                        if (errno == EAGAIN || errno == EWOULDBLOCK) {
                            break;
                        } else {
                            goto init_chatty_io_fatal;
                        }
                    }
                    // set nonblocking ET mode
                    GOTO_IF(set_non_blocking(conn_sock) == -1, init_chatty_io_fatal);

                    ev.events = EPOLLOUT | EPOLLIN | EPOLLET;
                    ev.data.fd = conn_sock;
                    GOTO_IF(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, conn_sock, &ev) == -1, init_chatty_io_fatal);
                    printf("epoll add fd : %d\n", conn_sock);
                }
            } else {
                if ((events[n].events & EPOLLIN) == EPOLLIN) {
                    epoll_in_handler(events[n]);
                } else if ((events[n].events & EPOLLOUT) == EPOLLOUT) {
                    epoll_out_handler(events[n]);
                }
            }
        }
    }

    return;
init_chatty_io_fatal:
    perror("error in init_chatty_io");
    perror(strerror(errno));
    free_global_variables();
    exit(EXIT_FAILURE);
}



void
epoll_in_handler(struct epoll_event ev)
{
    static __thread int a;
    static __thread int buf_size = 4096;
    static __thread unsigned char* buf;
    static __thread int rs;
    buf = (unsigned char*)malloc(buf_size + 1);
    GOTO_IF(buf == NULL, epoll_in_handler_fatal);
    printf("eopll in  %d\n", ev.data.fd);
    for (;;){
        bzero(buf, buf_size + 1);
        rs = read(ev.data.fd, buf, buf_size);
        printf("%d\n", rs);
        if (rs == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                /*printf("break\n");*/
                break;
            } else {
                goto epoll_in_handler_fatal;
            }
        } else if (rs == 0) {
            /*printf("connection closed\n");*/
            close(ev.data.fd);
            break;
        }
        printf("%d bytes read from fd%d\n", rs, ev.data.fd);
        for (int i = 0; i < rs; i++) {
            printf("%02x ", buf[i]);
        }
        printf("\n%s\n", buf);
        printf("\n");
    }
    return;

epoll_in_handler_fatal:
    perror(strerror(errno));
    free(buf);
    exit(EXIT_FAILURE);
}
    
void
epoll_out_handler(struct epoll_event ev)
{
    printf("eopll out %d\n", ev.data.fd);
}
