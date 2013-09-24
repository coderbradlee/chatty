
#ifndef GLOBAL_H
#define GLOBAL_H

#include <errno.h>
#include <stdlib.h>
#include <string.h>

extern int ws_server_port;
extern int max_events;
extern struct epoll_event* events;

// fprintf("error occur in %s %d\n", #__FILE__, #__LINE__);
#define GOTO_IF_TRUE(true_condition, label)\
    if (true_condition) {\
        goto label;\
    }

#define GOTO_IF_FALSE(false_condition, label)\
    if (!(false_condition)) {\
        goto label;\
    }

#ifdef __cplusplus 
extern "C" {
#endif

void init_global_variables(void);
void free_global_variables(void);

#ifdef __cplusplus 
}
#endif

#endif
