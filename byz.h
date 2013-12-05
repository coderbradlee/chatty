
#ifndef BYZ_H
#define BYZ_H

#include <stdlib.h>
#include <stdint.h>

struct byz {
    size_t cur_len;
    size_t max_len;
    void* head;
    void* tail;
    void* data;
};


struct byz* byz_new(size_t init_len, size_t max_len);
struct byz* byz_new_fixed(size_t fixed_len);
struct byz* byz_new_nolimit(size_t init_len);
void byz_free(struct byz* buf);
size_t byz_size(struct byz* buf);
int byz_put(struct byz* buf, const void* src, size_t len);
int byz_get(struct byz* buf, void* dest, size_t len);

#endif
