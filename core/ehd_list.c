//
// Created by renmingxu on 12/21/17.
//


#include <ehd_config.h>
#include <ehd_core.h>

ehd_list_t* ehd_list_create(ehd_pool_t* pool, u_int n, size_t size) {
    ehd_list_t* list;

    if (!(list = ehd_palloc(pool, sizeof(ehd_list_t)))) {
        return NULL;
    }

    if (ehd_list_init(list, pool, n, size) != EHD_OK) {
        return NULL;
    }

    return list;
}

int ehd_list_init(ehd_list_t* list, ehd_pool_t* pool, u_int n, size_t size) {
    list->head.data = ehd_palloc(pool, n * size);

    if (list->head.data == NULL) {
        return EHD_ERROR;
    }

    list->head.ndata = 0;
    list->head.next = NULL;
    list->pool = pool;
    list->last = &list->head;
    list->ndata = n;
    list->size = size;
    return EHD_OK;
}

void* ehd_list_push(ehd_list_t* list) {
    void * data;
    ehd_list_node_t *last;

    last = list->last;

    if (last->ndata == list->ndata) {

        /*
         * the data block of the last node is full
         * allocate a new node
         */
        if (!(last = ehd_palloc(list->pool, sizeof(ehd_list_node_t)))) {
            return NULL;
        }
        if (!(last->data = ehd_palloc(list->pool, list->ndata * list->size))) {
            return NULL;
        }
        last->ndata = 0;
        last->next = NULL;

        list->last->next = last;
        list->last = last;
    }

    data = (char*) last->data + list->size * last->ndata;
    last->ndata++;

    return data;
}