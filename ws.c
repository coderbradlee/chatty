

#include <string.h>
#include <stdint.h>
#include "ws.h"

/**
 * If accept, return next start, else return NULL.
 */
char*
ws_parse_test_key(struct ws* frame, char* key, char* head, char* tail)
{
    if (strcmp(head, key) != 0) {
        return NULL;
    }
    while (head < tail && *head != '\r')
        head++;
    if (tail - head < 2 || *(head+1) != '\n')
        return NULL;
    (*(uint16_t*)head) = 0;
    return head+2;
}

char*
ws_parse(struct ws* frame, char* head, char* tail)
{
    if (!frame || !head || !tail) {
        return NULL;
    }
    if (tail - head < 2) {
        return NULL;  // must be invalid
    }
    frame->head = head;
    if (*head == 'G') {  // maybe handshake branch
        // first line must be GET

    } else if ((*(head+1)) >> 7) {  // come from client, must be 1
        // client package
    }
    return NULL;
}
