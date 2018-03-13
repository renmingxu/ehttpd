//
// Created by renmingxu on 12/25/17.
//

#ifndef EHTTPD_EHD_ARRAY_H
#define EHTTPD_EHD_ARRAY_H

#include <ehd_config.h>
#include <ehd_core.h>

typedef struct ehd_array_s ehd_array_t;

struct ehd_array_s {
    void * data;
    size_t size;
    uint nblock;
    uint ndata;
    ehd_pool_t * pool;
};

ehd_array_t *ehd_array_create(ehd_pool_t * pool,  uint n, size_t size);
inline static int ehd_array_init(ehd_array_t *array, ehd_pool_t *pool, uint n, size_t size) {
    if (!(array->data = ehd_palloc(pool, n * size))) {
        return EHD_ERROR;
    }
    array->ndata = 0;
    array->size = size;
    array->nblock = n;
    array->pool = pool;
}

typedef struct {
    ehd_str_t key;
    ehd_str_t value;
} ehd_table_data_t;
void ehd_array_destroy(ehd_array_t *array);
void * ehd_array_push(ehd_array_t *array);

#endif //EHTTPD_EHD_ARRAY_H
