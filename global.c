
#include "global.h"


/**
 * Define global varialbes
 */
int ws_server_port;
int max_events;
struct epoll_event* events;

void
init_global_variables(void)
{
    ws_server_port = 50000;
    max_events = 100;
    events = calloc(max_events, sizeof(struct epoll_event));
    GOTO_IF(events == NULL, init_global_variables_fatal);
    bzero(events, sizeof(struct epoll_event) * max_events);
    return;
init_global_variables_fatal:
    perror(strerror(errno));
    free_global_variables();
    exit(EXIT_FAILURE);
}

void
free_global_variables(void)
{
    free(events);
}
