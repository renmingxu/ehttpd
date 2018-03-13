//
// Created by renmingxu on 12/25/17.
//

#include <ehd_config.h>
#include <ehd_core.h>
#include <test_all.h>

int test_pool() {
    ehd_pool_t *pool = ehd_create_pool_default_size();
    ehd_pool_t pool1;
    int *n;
    n = ehd_palloc(pool, sizeof(int) * 100000);
    for(int i = 0;i < 100000;i++) {
        n[i] = i;
    }
    for(int i = 0;i < 100000;i++) {
        if (n[i] != i)
            return -1;
    }
    n = ehd_palloc(pool, sizeof(int) * 100000);
    for(int i = 0;i < 100000;i++) {
        n[i] = i;
    }
    for(int i = 0;i < 100000;i++) {
        if (n[i] != i)
            return -1;
    }
    ehd_destroy_pool(pool);
    return 0;
}
int test_array() {
    ehd_pool_t * pool = ehd_create_pool_default_size();
    ehd_array_t * array = ehd_array_create(pool, 100, sizeof(int));
    for(int i = 0;i < 522241;i ++) {
        *(int*)ehd_array_push(array) = i;
    }
    printf("array size %d\n", array->nblock);
}
int test_list() {
    ehd_pool_t * pool = ehd_create_pool_default_size();
    ehd_list_t * list = ehd_list_create(pool, 256, sizeof(int));
    for(int i = 0;i < 5000;i++) {
        *(int*)ehd_list_push(list) = i;
    }
    ehd_list_node_t * node;
    int n = 0;
    int m = 0;
    int * p;
    for (node = &list->head;node != NULL;node = node->next) {
        for(int n = 0;n < node->ndata;n++,m++) {
            p = (int*)(node->data + n * list->size);
            if (*p != m) {
                return EHD_ERROR;
            }
        }
    }
}
int test_all() {
    int n = 0;
    if (test_pool() < 0){
        return -1;
    }
    n++;
    if (test_array() < 0){
        return -1;
    }
    n++;
    if (test_list() < 0){
        return -1;
    }
    n++;
    return n;
}


