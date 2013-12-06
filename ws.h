
#ifndef WS_H
#define WS_H


#include <stdint.h>

#define WS_INVALID 0
#define WS_HANDSHAKE 1
#define WS_DATA 2
#define WS_KEY_MAX_LEN 128

struct ws_frame {  // on the fly parsing, no data is stored by self
    uint8_t type;
    char* head;
    char* tail;
    union {
        struct{
            char* get;
            char* upgrade;
            char* connection;
            char* host;
            char* origin;
            char* pragma;
            char* cache_control;
            char* sec_websocket_key;
            char* sec_websocket_version;
            char* user_agent;
        };

        struct{
            uint8_t fin;
            uint8_t op_code;
            uint8_t mask;
            uint64_t payload_length;
            char mask_key[4];
            char* data_head;
        };
    };
};


/**
 * Start from head, stop no more than tail.
 * Return the end pointer of this frame.
 * Return NULL on error.
 */
char* ws_parse(struct ws_frame* frame, char* head, char* tail);

char* ws_answer_key(char* key);


#endif
