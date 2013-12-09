
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include "ecpoll.h"
#include "chatty.h"

struct ecpoll_keeper* keeper;


int
interrupt_handler(sig_t s)
{
    printf("cleaning up...\n");
    ecpoll_keeper_stop(keeper);
    ecpoll_keeper_free(keeper);
    printf("Bye :)\n");
    close(0);
    close(1);
    close(2);
    exit(EXIT_SUCCESS);
    return 0;
}

int main(int argc, const char *argv[])
{
    signal(SIGINT, interrupt_handler);

    keeper = ecpoll_keeper_new(4);
    struct ecpoll_inst* inst = ecpoll_inst_new(32,
            chatty_startup, chatty_on_connect,
            chatty_on_receive, chatty_cleanup);
    inst->server_port = 5000;
    ecpoll_keeper_register_instance(keeper, inst);

    ecpoll_keeper_prepare(keeper);
    ecpoll_keeper_start(keeper);

    while (1)
        usleep(200);

    return 0;
}
