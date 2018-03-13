//
// Created by renmingxu on 12/21/17.
//

#include <ehd_config.h>
#include <ehd_core.h>

ehd_pool_t *ehd_create_pool(size_t size) {
    ehd_pool_t *p;

    /* malloc a block of memory to store pool struct and data */
    if (!(p = malloc(size))) {
        return NULL;
    }

    p->last = (char *)p + sizeof(ehd_pool_t);
    p->end = (char *)p + size;
    p->next = NULL;
    p->large = NULL;

    return p;
}

void ehd_destroy_pool(ehd_pool_t *pool) {
    ehd_pool_t *p, *n;
    ehd_pool_large_t *l;

    for (l = pool->large; l; l = l->next) {
        if (l->data) {
            free(l->data);
        }
    }
    for (p = pool, n = pool->next; /* void */; p = n, n = n->next) {
        free(p);

        if (n == NULL) {
            break;
        }
    }
}

void *ehd_palloc(ehd_pool_t *pool, size_t size) {
    char *m;
    ehd_pool_t *p, *n;
    ehd_pool_large_t *large, *last;


    /* check if the size can be put in a pool */
    if (size <= (size_t)EHD_MAX_ALLOC_FROM_POOL &&
        size <= (size_t)(pool->end - (char *)pool) - sizeof(ehd_pool_t)) {
        /* find a pool which has enough space and return it p->last */
        for (p = pool, n = pool->next; /* void */; p = n, n = n->next) {
            m = p->last;

            if (size <= (size_t)(p->end - m)) {
                p->last = m + size;
                return m;
            }
            if (n == NULL) {
                break;
            }
        }

        /* allocate a new pool block */
        if (!(n = ehd_create_pool((size_t)(p->end - (char *)p)))) {
            return NULL;
        }

        p->next = n;
        m = n->last;
        n->last += size;

        return m;
    }

    /* allocate a large block */


    large = NULL;
    last = NULL;

    if (pool->large) {
        for (last = pool->large; /* void */ ; last = last->next) {
            if (last->data == NULL) {
                large = last;
                last = NULL;
                break;
            }

            if (last->next == NULL) {
                break;
            }
        }
    }

    if (large == NULL) {
        if (!(large = ehd_palloc(pool, sizeof(ehd_pool_large_t)))) {
            return NULL;
        }

        large->next = NULL;
    }

    if (!(m = malloc(size))) {
        return NULL;
    }

    if (pool->large == NULL) {
        pool->large = large;

    } else if (last) {
        last->next = large;
    }

    large->data = m;

    return m;
}

void *ehd_pcalloc(ehd_pool_t * pool, size_t size) {
    void *m;

    if (!(m = ehd_palloc(pool,size))) {
        return NULL;
    }

    memset(m,0,size);

    return m;
}

ehd_pool_t * ehd_create_pool_default_size() {
    return ehd_create_pool(EHD_DEFAULT_POOL_SIZE);
}

