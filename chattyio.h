
#ifndef CHATTYIO_H
#define CHATTYIO_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <netdb.h>
#include <strings.h>
#include "global.h"

#ifdef __cplusplus 
extern "C" {
#endif

void init_chatty_io(void);

#ifdef __cplusplus 
}
#endif



#endif
