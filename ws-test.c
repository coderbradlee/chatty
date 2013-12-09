
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ws.h"


char* invalid[4] = {
"GET /123/123/123/123/123 HTTP/1.1\r\n"
"Host: localhost:12345\r\n"
"User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:25.0) Gecko/20100101 Firefox/25.0\r\n"
"Cache-Control: no-cache\r\n"
"Upgrade: websocket\r\n"
"\r\n",

"GET /123/123/123/123/123 HTTP/1.1\r"
"Host: localhost:12345\r\n"
"User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:25.0) Gecko/20100101 Firefox/25.0\r\n"
"Cache-Control: no-cache\r\n"
"Upgrade: websocket\r\n"
"\r\n",

"",  //empty test

"GET /123/123/123/45 HTTP/1.1\r\n"
"Upgrade: websocket\r\n"
"Connection: Upgrade\r\n"
"Host: localhost:12345\r\n"
"Origin: http://www.cplusplus.com\r\n"
"Pragma: no-cache\r\n"
"Cache-Control: no-cache\r\n"
"Sec-WebSocket-Key: gxeAghYpJSLOiT4/18T2jQ==\r\n"
"Sec-WebSocket-Version: 13\r\n"
"Sec-WebSocket-Extensions: x-webkit-deflate-frame\r\n"
"User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/31.0.1650.57 Safari/537.36\r\n"  // no "\r\n" at the end

};

#define NEQ(a, b) \
    if ((a) == (b)) {\
        fprintf(stderr, "[Fail] at line : %d\n", __LINE__);\
    } else {\
        fprintf(stdout, "[Pass]\n");\
    }

#define EQ(a, b) \
    if ((a) != (b)) {\
        fprintf(stderr, "[Fail] at line : %d\n", __LINE__);\
    } else {\
        fprintf(stdout, "[Pass]\n");\
    }
#define EQSTR(a, b)  EQ(strcmp((a), (b)), 0)


int main(int argc, const char *argv[])
{
    char* head;
    char* tail;
    struct ws_frame* ws;
    ws = malloc(sizeof(struct ws_frame));

    char chrome_handshake[] = 
    "GET /123/123/123/45 HTTP/1.1\r\n"
    "Upgrade: websocket\r\n"
    "Connection: Upgrade\r\n"
    "Host: localhost:12345\r\n"
    "Origin: http://www.cplusplus.com\r\n"
    "Pragma: no-cache\r\n"
    "Cache-Control: no-cache\r\n"
    "Sec-WebSocket-Key: gxeAghYpJSLOiT4/18T2jQ==\r\n"
    "Sec-WebSocket-Version: 13\r\n"
    "Sec-WebSocket-Extensions: x-webkit-deflate-frame\r\n"
    "User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/31.0.1650.57 Safari/537.36\r\n"
    "\r\n";


    head  = strdup(chrome_handshake);
    tail = head + strlen(head);
    ws_parse(ws, head, tail);
    EQSTR(ws->get, "/123/123/123/45 HTTP/1.1");
    EQSTR(ws->upgrade, "websocket");
    EQSTR(ws->connection, "Upgrade");
    EQSTR(ws->host, "localhost:12345");
    EQSTR(ws->origin, "http://www.cplusplus.com");
    EQSTR(ws->sec_websocket_version, "13");
    EQSTR(ws->sec_websocket_key, "gxeAghYpJSLOiT4/18T2jQ==");
    free(head);

    char firefox_handshake[] = 
    "GET /123/123/123/123/123 HTTP/1.1\r\n"
    "Host: localhost:12345\r\n"
    "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:25.0) Gecko/20100101 Firefox/25.0\r\n"
    "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
    "Accept-Language: en-US,en;q=0.5\r\n"
    "Accept-Encoding: gzip, deflate\r\n"
    "Sec-WebSocket-Version: 13\r\n"
    "Origin: null\r\n"
    "Sec-WebSocket-Key: z5xlNzJeVBelmrZsKECQ1w==\r\n"
    "Connection: keep-alive, Upgrade\r\n"
    "Pragma: no-cache\r\n"
    "Cache-Control: no-cache\r\n"
    "Upgrade: websocket\r\n"
    "\r\nMOREDATA";

    head  = strdup(firefox_handshake);
    tail = head + strlen(head);
    ws_parse(ws, head, tail);
    EQSTR(ws->get, "/123/123/123/123/123 HTTP/1.1");
    EQSTR(ws->upgrade, "websocket");
    EQSTR(ws->connection, "keep-alive, Upgrade");
    EQSTR(ws->host, "localhost:12345");
    EQSTR(ws->origin, "null");
    EQSTR(ws->sec_websocket_version, "13");
    EQSTR(ws->sec_websocket_key, "z5xlNzJeVBelmrZsKECQ1w==");
    free(head);

    int i = 0;
    char* re;
    for (i = 0; i < 4; i++) {
        head = strdup(invalid[i]);
        tail = head + strlen(invalid[i]);
        re = ws_parse(ws, head, tail);
        EQ(re, NULL);
        free(head);
    }

    head =  ws_answer_key("dGhlIHNhbXBsZSBub25jZQ==");
    EQSTR(head, "s3pPLMBiTxaQ9kYGzzhZRbK+xOo=");
    free(head);

    head = ws_answer_frame("dGhlIHNhbXBsZSBub25jZQ==");
    printf("%s", head);
    free(head);

    free(ws);
    return 0;
}
