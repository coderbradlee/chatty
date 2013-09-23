
#ifndef CHATTYIO_H
#define CHATTYIO_H

#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <netdb.h>

#ifdef __cplusplus 
extern "C" {
#endif

void chatty_io_init(void);

#ifdef __cplusplus 
}
#endif



#endif
