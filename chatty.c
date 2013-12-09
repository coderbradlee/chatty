
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include "ecpoll.h"
#include "chatty.h"
#include "byz.h"
#include "ws.h"

int
chatty_on_connect(struct epoll_event ev, struct ecpoll_inst* inst)
{
    int fd = ev.data.fd;
    struct chatty_channel* channels =
        (struct chatty_channel*)(inst->data);
    channels[fd].status = CHATTY_NEED_AUTH;
    byz_clear(channels[fd].in_data);
    byz_clear(channels[fd].out_data);
    time(&channels[fd].last_active);
    printf("on connect fd %d: \n", ev.data.fd);
    return 0;
}


int
chatty_on_receive(struct epoll_event ev, struct ecpoll_inst* inst)
{
    char tmp[4096];
    printf("on receive fd %d: \n", ev.data.fd);
    struct chatty_channel* channels =
        (struct chatty_channel*)inst->data;
    int fd = ev.data.fd;
    if (ev.events & EPOLLIN) {
        int cnt;
        while (1) {
            cnt = read(ev.data.fd, tmp, 4096);
            if (cnt == -1) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    break; // normal, just break
                }
                goto read_fatal;
            }
            if (byz_put(channels[fd].in_data, tmp, cnt))
                goto read_buffer_reach_max;
        }

        struct ws_frame ws;
        void* new_head = ws_parse(&ws, channels[fd].in_data->head,
                channels[fd].in_data->tail);
        if (!new_head) {
            // wait for next read.
        }
        if (ws.type & WS_HANDSHAKE) {
            // accept anyway
            char* resp = ws_answer_frame(ws.sec_websocket_key);
            write(ev.data.fd, resp, strlen(resp));
            free(resp);
        } else if (ws.type & WS_DATA) {
        }
        printf("un implemented\n");
        goto read_fatal;

    }
    //if (ev.events & EPOLLOUT) {
    //}
    return 0;

// error handle branch
read_buffer_reach_max:
read_fatal:
    printf("close fd:%d\n", ev.data.fd);
    close(ev.data.fd);
    channels[fd].status = 0;
    byz_clear(channels[fd].in_data);
    byz_clear(channels[fd].out_data);
    return 0;
}


void
chatty_startup(struct ecpoll_inst* inst)
{
    struct chatty_channel* channels =
        (struct chatty_channel*)malloc(sizeof(struct chatty_channel) * MAX_FD_ALLOWED);
    memset(channels, 0, sizeof(struct chatty_channel) * MAX_FD_ALLOWED);
    int i = 3;
    for (; i < MAX_FD_ALLOWED; i++) {
        channels[i].fd = i;
        channels[i].in_data = byz_new(4096, 65536);  //max 64kb.
        channels[i].out_data = byz_new(4096, 65536);
    }
    inst->data = channels;
}

void
chatty_cleanup(void* data)
{
    if (data) {
        struct chatty_channel* channels = (struct chatty_channel*)data;
        int i = 3;
        for (; i < MAX_FD_ALLOWED; i++) {
            if (channels[i].status)
                close(i);
            byz_free(channels[i].in_data);
            byz_free(channels[i].out_data);
        }
        free(data);
    }
}
