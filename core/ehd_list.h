//
// Created by renmingxu on 12/21/17.
//

#ifndef UNTITLED_EHD_LIST_H
#define UNTITLED_EHD_LIST_H

#include <ehd_config.h>
#include <ehd_core.h>

typedef struct ehd_list_node_s ehd_list_node_t;

struct ehd_list_node_s {
    /* ptr to ndata * size block */
    void *data;
    /* numbers of  elements used (no more than ndata)*/
    uint ndata;
    ehd_list_node_t *next;
};

struct ehd_list_s {
    /* end of node list */
    ehd_list_node_t *last;
    /* head of node list */
    ehd_list_node_t head;
    /* sizeof value type in list */
    size_t size;
    /* numbers of element in each node */
    unsigned int ndata;
    /* ehd_pool_t used by this list */
    ehd_pool_t *pool;
};

/* create a list */
ehd_list_t *ehd_list_create(ehd_pool_t *pool, u_int n, size_t size);

/*
 * NOT need to call after ehd_list_create
 * call after an ehd_list_t type is member of a struct
 */
int ehd_list_init(ehd_list_t *list, ehd_pool_t *pool, u_int n, size_t size);

/* add a ehd_list_node_t after last */
void *ehd_list_push(ehd_list_t *list);
#endif //UNTITLED_EHD_LIST_H
