#include <stdio.h>
#include <stdlib.h>
#include "global.h"
#include "chattyio.h"

int main(int argc, const char *argv[])
{
    printf("chatty start...\n");
    init_global_variables();
    init_chatty_io();
    atexit(free_global_variables);
    return 0;
}
