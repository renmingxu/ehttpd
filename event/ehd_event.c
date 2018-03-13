//
// Created by renmingxu on 12/21/17.
//

#include <ehd_config.h>
#include <ehd_core.h>

ehd_event_impl_t ehd_event_impl;

int ehd_event_init(ehd_cycle_t *cycle) {

    uint i;
    int fd;
    ehd_conf_t * conf;
    ehd_pool_t * pool;
    ehd_connection_t * conns;
    ehd_event_t * evs;

    ehd_listening_t *l;
    ehd_connection_t *c;
    if (cycle->pool == NULL) {
        cycle->pool = ehd_create_pool_default_size();
    }
    pool = cycle->pool;
    conf = cycle->conf;
    uint conn_n = conf->connection_num;

    cycle->connection_num = conn_n;
    ehd_return_if_null(cycle->connections, ehd_palloc(pool, sizeof(ehd_connection_t) * conn_n), EHD_ERROR);
    conns = cycle->connections;
    ehd_return_if_null(cycle->events, ehd_palloc(pool, sizeof(ehd_event_t) * conn_n), EHD_ERROR);
    evs = cycle->events;

    for (i = 0;i < conn_n;i++) {
        conns[i].fd = -1;
        conns[i].data = NULL;
    }

    for (i = 0;i < conn_n;i++) {
        evs[i].data = NULL;
        evs[i].closed = 1;
    }

    for (i = 0;i < conn_n;i++) {
        evs[i].data = NULL;
        evs[i].closed = 1;
    }

    for (i = 0;i < cycle->listening.ndata;i++) {
        l = cycle->listening.data + i * cycle->listening.size;
        fd = l->fd;
        c = conns + fd;
        evs = cycle->events + fd;

        bzero(evs, sizeof(ehd_event_t));
        bzero(c, sizeof(ehd_connection_t));
        evs->accept = 1;
        evs->data = c;
        evs->closed = 0;

        c->fd = fd;
        c->event = evs;
        c->listening = l;
        c->read_handler = ehd_event_accept_handler;
    }

    ehd_event_impl = ehd_event_impl_epoll;

    ehd_event_impl.funp.init(cycle);

    return EHD_OK;
}

