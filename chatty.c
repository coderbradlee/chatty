
#include "ecpoll.h"
#include "chatty.h"

int
chatty_on_receive(struct epoll_event ev, struct ecpoll_inst* inst)
{
    return 0;
}


void*
chatty_startup(struct ecpoll_inst* data)
{
    return NULL;
}


void
chatty_cleanup(void* data)
{
}
