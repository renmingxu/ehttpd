//
// Created by renmingxu on 12/21/17.
//


#include <ehd_config.h>
#include <ehd_core.h>

ehd_buf_t* ehd_create_temp_buf(ehd_pool_t *pool, size_t size) {
    ehd_buf_t* b;
    if (!(b = ehd_pcalloc(pool, sizeof(ehd_buf_t)))) {
        return NULL;
    }
    if (!(b->start = ehd_palloc(pool, size))) {
        return NULL;
    }
    b->pos = b->start;
    b->last = b->start;
    b->end = b->start + size;
    b->temporary = 1;
    return b;
}

