#ifndef CHATTY_H
#define CHATTY_H

#include <stdint.h>
#include "byz.h"


#define MAX_USER_ALLOWED 1024
#define MAX_MSGS_ALLOWED 100

#define ROOM_ID_LEN 3
#define USER_ID_LEN 3

struct chatty_user {
    int fd;
    char user_id[USER_ID_LEN];
    uint8_t status;
};

struct chatty_room {
    char   room_id[ROOM_ID_LEN];
    char** user_ids;
};

struct chatty_msg {
    char room_id[ROOM_ID_LEN];  // from which room
    char user_id[USER_ID_LEN];      // from who
    struct byz* words;       // say what?
};


// msg -> room -> users -> broadcast

extern struct chatty_user users[MAX_USER_ALLOWED];

#endif
