//
// Created by renmingxu on 1/2/18.
//
#include <ehd_config.h>
#include <ehd_core.h>
#include <ehd_http.h>

int ehd_http_init(ehd_cycle_t * cycle){
    uint i;
    ehd_listening_t * ls;
    for(i = 0;i < cycle->listening.ndata;i++) {
        ls = cycle->listening.data;
        ls[i].handler =  ehd_http_init_connection;
    }
    return EHD_OK;
}

void ehd_http_empty_handler(ehd_connection_t *c) {
    return;
}

void ehd_http_close_connection(ehd_connection_t *c) {
    ehd_close_connection(c);
}
