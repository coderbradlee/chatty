#include <stdio.h>
#include <stdlib.h>
#include "global.h"


int main(int argc, const char *argv[])
{
    init_global_variables();
    printf("hello\n");
    printf("%s\n", ws_server_port);
    return 0;
}
