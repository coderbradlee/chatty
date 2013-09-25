
#ifndef GLOBAL_H
#define GLOBAL_H

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/epoll.h>

extern int ws_server_port;
extern int max_events;
extern struct epoll_event* events;

// fprintf("error occur in %s %d\n", #__FILE__, #__LINE__);
#define GOTO_IF(true_condition, label)\
    if (true_condition) { goto label; }

#ifdef __cplusplus 
extern "C" {
#endif

void init_global_variables(void);
void free_global_variables(void);

#ifdef __cplusplus 
}
#endif

#endif
