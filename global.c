
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
    events = NULL;
}

void
free_global_variables(void)
{
    free(events);
}
