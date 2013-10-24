

#include "bbuf.h"
#include <string.h>
#include <strings.h>

bbuf_t*
bbuf_new(int init_capacity, int mode)
{
    bbuf_t* m = (bbuf_t*)malloc(sizeof(bbuf_t));
    if (!m) {
        return NULL;
    }
    m->mode = mode;
    m->capacity = init_capacity;
    m->buf = (char*)malloc(m->capacity);
    if (!m->buf) {
        free(m);
        return NULL;
    }
    bbuf_reset(m);
    return m;
}

void
bbuf_reset(bbuf_t* self)
{
    bzero(self->buf, self->capacity);
    self->read_ptr = self->buf;
    self->write_ptr = self->buf;
}

void
bbuf_free(bbuf_t* self)
{
    free(self->buf);
    free(self);
}

int
bbuf_is_empty(bbuf_t* self)
{
    return !(self->write_ptr - self->read_ptr);
}

int
bbuf_is_full(bbuf_t* self)
{
    if (self->mode == STMB_MODE_AUTOEXPAND) {
        return 0;
    }
    return (self->write_ptr - self->buf) == self->capacity;
}

int
bbuf_feed(bbuf_t* self, char* src, int size)
{
    if (self->buf + self->capacity >= self->write_ptr + size - 1) {
    // space enough, don't need to move.
    }
}

int
bbuf_grab(bbuf_t* self, char* dest, int size)
{
}
