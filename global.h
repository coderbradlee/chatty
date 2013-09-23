
#ifndef GLOBAL_H
#define GLOBAL_H

#include <sys/epoll.h>

extern char* ws_server_port;
extern int max_events;
extern struct epoll_event* events;

#define GOTO_IF_TRUE(true_condition, label)\
    if (true_condition) { goto label; }

#define GOTO_IF_FALSE(false_condition, label)\
    if (!(false_condition)) { goto label; }

#ifdef __cplusplus 
extern "C" {
#endif

void init_global_variables(void);

#ifdef __cplusplus 
}
#endif

#endif
