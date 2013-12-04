

#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "byz.h"

struct byz*
byz_new(size_t init_len, size_t max_len)
{
    struct byz* buf = (struct byz*)malloc(sizeof(struct byz));
    if (!buf)
        return NULL;
    bzero(buf, sizeof(struct byz));
    buf->data = malloc(init_len);
    if (!buf->data)
        goto byz_new_err1;
    bzero(buf->data, init_len);
    buf->cur_len = init_len;
    buf->max_len = max_len;
    buf->head = buf->tail = buf->data;
    return buf;

byz_new_err1:
    free(buf);
    return NULL;
}


struct byz*
byz_new_fixed(size_t fixed_len)
{
    return byz_new(fixed_len, fixed_len);
}


struct byz*
byz_new_nolimit(size_t init_len)
{
    return byz_new(init_len, -1);
}


void
byz_free(struct byz* buf)
{
    if (buf && buf->data) {
        free(buf->data);
        free(buf);
    }
}


size_t
byz_size(struct byz* buf)
{
    return buf->tail - buf->head;
}

size_t
byz_put(struct byz* buf, const void* src, size_t len)
{
    if (buf->tail + len <= buf->data + buf->cur_len) {
        // no expand, no shift
        memcpy(buf->tail, src, len);
        buf->tail += len;
        return 0;
    }

    size_t head_offset, tail_offset;
    void* new_data;
    size_t req_size = buf->tail - buf->head + len;
    size_t new_len = buf->cur_len;
    if (req_size > buf->max_len) {
        // not possible for holding all content
        return -1;
    }
    while ((new_len <<= 1) < req_size);
    if (new_len > buf->max_len) {
        new_len = buf->max_len;
    }
    buf->cur_len = new_len;

    // can't expand but shift
    new_data = malloc(buf->cur_len);
    if (!new_data)
        return -1;
    memcpy(new_data, buf->head, buf->tail - buf->head);
    buf->tail = new_data + (buf->tail - buf->head);
    buf->head = new_data;
    free(buf->data);
    buf->data = new_data;
    memcpy(buf->tail, src, len);
    buf->tail += len;
    return 0;
}


size_t
byz_get(struct byz* buf, void* dest, size_t len)
{
    if (buf->tail - buf->head >= len) {
        memcpy(dest, buf->head, len);
        buf->head += len;
        if (buf->head - buf->data > buf->cur_len/2) {
            size_t size = buf->tail - buf->head;
            memcpy(buf->data, buf->head, size);
            buf->head = buf->data;
            buf->tail = buf->head + size;
        }
        return 0;
    }
    return -1;
}
