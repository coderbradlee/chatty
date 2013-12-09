

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "ws.h"
#include "byz.h"
#include "sha1.h"
#include "base64.h"


static char magic[] = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";


/**
 * If accept, return next start, else return NULL.
 */
char*
ws_parse(struct ws_frame* frame, char* head, char* tail)
{
    if (!frame || !head || !tail) {
        return NULL;
    }
    if (tail - head < 2) {
        return NULL;  // must be invalid
    }
    memset(frame, 0, sizeof(struct ws_frame));
    if (*head == 'G') {  // maybe handshake branch
#define WS_PARSE_TRY_KEY(target, cmp_len, key_name, offset_if_match); \
        if (!memcmp(head, target, (size_t)cmp_len)) {\
            head += offset_if_match;\
            frame->key_name = head;\
            continue;\
        }
        if (tail - head >= 4 && memcmp(head, "GET ", 4))
            return NULL;
        frame->type = WS_HANDSHAKE;
        frame->get = head + 4;
        head += 5;
        for (;head < tail;) {
            // locate the next "\r\n" within boundary
            while (head != tail && *head !='\r')
                head++;
            if (tail - head >= 2 && *(head+1) == '\n') {
                *head++ = '\0';
                *head++ = '\0';
            } else {
                goto handshake_error;
            }
            WS_PARSE_TRY_KEY("Upgrade: websocket", 18, upgrade, 9);
            WS_PARSE_TRY_KEY("Connection: ", 12, connection, 12);
            WS_PARSE_TRY_KEY("Host: ", 6, host, 6);
            WS_PARSE_TRY_KEY("Origin: ", 8, origin, 8);
            WS_PARSE_TRY_KEY("Pragma: ", 8, pragma, 8);
            WS_PARSE_TRY_KEY("Cache-Control: ", 15, cache_control, 15);
            WS_PARSE_TRY_KEY("Sec-WebSocket-Key: ", 19, sec_websocket_key, 19);
            WS_PARSE_TRY_KEY("Sec-WebSocket-Version: 13", 25, sec_websocket_version, 23);
            WS_PARSE_TRY_KEY("User-Agent: ", 12, user_agent, 12);
            if (tail - head >= 2 && *(head) == '\r' && *(head+1) == '\n') {
                (*(uint16_t*)head) = 0;
                head += 2;
                goto handshake_test;
            }

        }
        goto handshake_error;
handshake_test:
        #define WS_PARSE_TEST_VALUE(key) (frame->key && *(frame->key))
        if (WS_PARSE_TEST_VALUE(upgrade) &&
            WS_PARSE_TEST_VALUE(host) &&
            WS_PARSE_TEST_VALUE(sec_websocket_version) &&
            WS_PARSE_TEST_VALUE(origin) &&
            WS_PARSE_TEST_VALUE(connection) &&
            WS_PARSE_TEST_VALUE(sec_websocket_key) &&
            strlen(frame->sec_websocket_key) < WS_KEY_MAX_LEN){  // length should be enough
            return frame;
        }
handshake_error:
        memset(frame, 0, sizeof(struct ws_frame));
        return NULL;

    } else if ((*(head+1)) >> 7) {  // come from client, must be 1
        // client package
        frame->fin = (*head) >> 7;
        frame->op_code = (*head) & 0x0f;
        head++;
        frame->mask = (*head) >> 7;
        uint8_t tmp = (*head) & 0x7f;
        head++;
        switch (tmp) {
            case 0 ... 125:
                frame->payload_length = tmp;
            case 126:
                if (tail - head < 2 + 4)
                    return NULL;
                frame->payload_length = ntohs(*head);
                head += 2;
            case 127:
                if (tail - head < 8 + 4)
                    return NULL;
                frame->payload_length = (uint64_t)(ntohl(*head)) << 32 |
                    ntohl(*(head+4));
                head += 8;
            default:
                return NULL;
        }
        memcpy(frame->mask_key, head, 4);
        head += 4;
        return head;
    }
    return NULL;
}


char*
ws_answer_key(char* key)
{
    if (!key || !*key)
        return NULL;

    size_t key_len = strlen(key);
    if (key_len >= WS_KEY_MAX_LEN)
        return NULL;

    char* tmp = malloc(256);
    if (!tmp)
        return NULL;
    memset(tmp, 0, 256);
    memcpy(tmp, key, key_len);
    strcat(tmp + key_len, magic);
    blk_SHA_CTX sha1_ctx;
    blk_SHA1_Init(&sha1_ctx);
    blk_SHA1_Update(&sha1_ctx, tmp, strlen(tmp));
    free(tmp);

    char* accept = malloc(30);
    if (!accept)
        return NULL;
    unsigned char sha1_hash[20];
    blk_SHA1_Final(sha1_hash, &sha1_ctx);
    memset(accept, 0, 30);
    Base64encode(accept, (char*)sha1_hash, 20);
    return accept;
}


char*
ws_answer_frame(char* key)
{
    static char answer_frame_tmpl[] =
    "HTTP/1.1 101 Switching Protocols\r\n"
    "Upgrade: websocket\r\n"
    "Connection: Upgrade\r\n"
    "Sec-WebSocket-Accept: %s\r\n\r\n";

    char* answer = (char*)malloc(256);
    if (!answer)
        return NULL;
    memset(answer, 0, 256);
    char* answer_key = ws_answer_key(key);
    if (!answer_key) {
        free(answer);
        return NULL;
    }
    sprintf(answer, answer_frame_tmpl, answer_key);
    free(answer_key);
    return answer;
}
