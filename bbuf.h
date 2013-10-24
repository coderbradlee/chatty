

#ifndef BBUF_H
#define BBUF_H

#include <stdlib.h>

#define STMB_MODE_FIXED 0
#define STMB_MODE_AUTOEXPAND 1

typedef struct bbuf {
    char* buf;
    char* read_ptr;
    char* write_ptr;
    int mode;
    int capacity;
} bbuf_t;


#ifdef __cplusplus 
extern "C" {
#endif

bbuf_t* bbuf_new(int init_max_size, int mode);
void bbuf_reset(bbuf_t* self);
void bbuf_free(bbuf_t* self);
int bbuf_is_empty(bbuf_t* self);
int bbuf_is_full(bbuf_t* self);
int bbuf_feed(bbuf_t* self, char* src, int size);
int bbuf_grab(bbuf_t* self, char* dest, int size);


#ifdef __cplusplus 
}
#endif



#endif
