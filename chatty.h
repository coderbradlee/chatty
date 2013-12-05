#ifndef CHATTY_H
#define CHATTY_H

#include <sys/types.h>
#include <stdint.h>
#include <time.h>
#include <json-c/json.h>
#include "byz.h"


#define MAX_USER_ALLOWED 1024
#define MAX_MSGS_ALLOWED 100

#define ROOM_ID_LEN 3
#define USER_ID_LEN 3


#define CHATTY_NEED_AUTH 1

struct chatty_channel {
    int fd;
    char room_id[ROOM_ID_LEN];
    char user_id[USER_ID_LEN];
    uint8_t status;
    time_t last_active;
    struct byz* in_data;  // one dimension buffer
};


// msg -> room -> users -> fds -> broadcast

int chatty_on_receive(struct epoll_event ev, struct ecpoll_inst* inst);
void* chatty_startup(struct ecpoll_inst*data);
void chatty_cleanup(void* data);




#endif
