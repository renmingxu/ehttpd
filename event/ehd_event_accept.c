//
// Created by renmingxu on 1/1/18.
//

#include <ehd_config.h>
#include <ehd_core.h>
#include <ehd_http.h>

void ehd_event_accept_handler(ehd_connection_t * c) {
    socklen_t len;
    ehd_sockaddr_t addr;
    ehd_pool_t *pool;
    int s, err;
    ehd_event_t * aev, *ev;
    ehd_connection_t *conn, *lconn;
    lconn = c;
    ev = c->event;
    pool =  NULL;

    while(1) {
        len = sizeof(ehd_sockaddr_t);

#ifdef _GNU_SOURCE
        s = accept4(lconn->fd, &addr.sockaddr, &len, SOCK_NONBLOCK);
#else
        s = accept(lconn->fd, &addr.sockaddr, &len);
#endif
        if (s == -1) {
            err = errno;
            if (err == EAGAIN) {
                return;
            }

            ev->disalbed = 1;
            ev->error = 1;
            return;
        }

        if (s >= ehd_cycle->connection_num) {
            close(s);
            return;
        }

        pool = ehd_create_pool(lconn->listening->pool_size);
        if (pool == NULL) {
            return;
        }

        if (ehd_setnonblocking(s) < 0){

            return;
        }

        conn = ehd_cycle->connections + s;
        aev = ehd_cycle->events + s;

        bzero(conn, sizeof(ehd_connection_t));
        bzero(aev, sizeof(ehd_event_t));

        conn->pool = pool;
        conn->listening = lconn->listening;
        conn->fd = s;
        conn->sockaddr = addr;
        conn->socklen  = len;
        conn->event = aev;

        aev->data = conn;
        aev->accept = 0;
        aev->active = 0;
        aev->events = 0;
        aev->closed = 0;

        conn->read_handler = lconn->listening->handler;
        conn->write_handler = NULL;
        ehd_add_event(aev, EHD_READ_EVENT, EPOLLET);
    }

}

int ehd_event_enable_accept(ehd_cycle_t *cycle) {
    uint i;
    ehd_listening_t *l;
    l = cycle->listening.data;
    for(i = 0;i < cycle->listening.ndata;i++) {
        if (ehd_add_event(&cycle->events[l[i].fd], EHD_READ_EVENT, EPOLLET) == EHD_ERROR) {
            return EHD_ERROR;
        }
    }
    return EHD_OK;
}

int ehd_event_disable_accept(ehd_cycle_t *cycle) {
    uint i;
    ehd_listening_t *l;
    for(i = 0;i < cycle->listening.ndata;i++) {
        if (!cycle->events[l[i].fd].active) {
            continue;
        }
        if (ehd_del_event(&cycle->events[l[i].fd], EHD_READ_EVENT,0) == EHD_ERROR) {
            return EHD_ERROR;
        }
    }
    return EHD_OK;
}

