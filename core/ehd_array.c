//
// Created by renmingxu on 12/25/17.
//

#include <ehd_config.h>
#include <ehd_core.h>

ehd_array_t * ehd_array_create(ehd_pool_t * pool, uint n, size_t size) {
    ehd_array_t *array = ehd_palloc(pool, sizeof(ehd_array_t));
    array->data = ehd_palloc(pool, size * n);
    array->size = size;
    array->nblock = n;
    array->ndata = 0;
    array->pool = pool;
    return array;
}

void ehd_array_destroy(ehd_array_t * array) {
    ehd_pool_t *p;
    p = array->pool;

    if ((char*)array->data + array->size * array->ndata == p->last) {
        p->last = array->data;
    }

    if ((char*) array + sizeof(ehd_array_t) == p->last) {
        p->last = (void*)array;
    }
}

void * ehd_array_push(ehd_array_t * array) {
    void * new, *data;
    ehd_pool_t *p;
    if (array->ndata == array->nblock) {
        p = array->pool;
        if ((char*)array->data + array->size * array->ndata == p->last
                && (unsigned)(p->end - p->last ) >= array->size) {
            p->last += array->size;
            array->nblock++;
        } else {
            ehd_return_if_null(new, ehd_palloc(p, 2 * array->size * array->nblock), NULL);

            memcpy(new, array->data, array->size * array->nblock);
            array->data = new;
            array->nblock *= 2;
        }
    }

    data = array->data + array->size * array->ndata;
    array->ndata ++;
    return data;
}


