//
// Created by renmingxu on 12/21/17.
//

#ifndef UNTITLED_EHD_PALLOC_H
#define UNTITLED_EHD_PALLOC_H

#include <ehd_config.h>
#include <ehd_core.h>

/* 1024 * 1024 */
#define EHD_MAX_ALLOC_FROM_POOL 1048576
/* 16 * 1024 */
#define EHD_DEFAULT_POOL_SIZE 16384

typedef struct ehd_pool_large_s ehd_pool_large_t;

struct ehd_pool_large_s {
    ehd_pool_large_t *next;
    void *data;
};


struct ehd_pool_s {
    char *last;
    char *end;
    ehd_pool_t *next;
    ehd_pool_large_t *large;
};


ehd_pool_t *ehd_create_pool(size_t size);
ehd_pool_t *ehd_create_pool_default_size();
void ehd_destroy_pool(ehd_pool_t *pool);

void * ehd_palloc(ehd_pool_t * pool, size_t size);
void * ehd_pcalloc(ehd_pool_t * pool, size_t size);
#endif //UNTITLED_EHD_PALLOC_H
