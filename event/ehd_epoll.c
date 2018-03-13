//
// Created by renmingxu on 12/31/17.
//

#include <ehd_config.h>
#include <ehd_core.h>
#include <ehd_epoll.h>

static int ehd_event_epoll_init(ehd_cycle_t * cycle){
    size_t n;
    int i;

    if (ep == -1) {
        ep = epoll_create(cycle->connection_num / 2);
        if (ep == -1) {
            return EHD_ERROR;
        }
    }

    if (nevents < cycle->connection_num) {
        if (event_list) {
            free(event_list);
        }
        event_list = malloc(sizeof(struct epoll_event) * cycle->connection_num);
        if (event_list == NULL) {
            return EHD_ERROR;
        }
    }

    nevents = cycle->connection_num;
    ehd_event_funps = ehd_event_impl_epoll.funp;

    return EHD_OK;
}

static int ehd_event_epoll_close(ehd_cycle_t * cyle) {
    if (close(ep) == -1) {
        return EHD_ERROR;
    }

    ep = -1;
    free(event_list);
}


static int ehd_event_epoll_mod(ehd_event_t * ev, int event, uint flags) {
    int op;
    ehd_connection_t * c;
    struct epoll_event ee;
    c = ev->data;
    op = EPOLL_CTL_MOD;

    /*
    if (ev->active) {
        op = EPOLL_CTL_MOD;
    } else {
        op = EPOLL_CTL_ADD;
        ev->active = 1;
    }
     */

    ee.events = event | flags;
    ee.data.ptr = c;

    ev->events = ee.events;

    if (epoll_ctl(ep, op, c->fd, &ee) == -1) {
        return EHD_ERROR;
    }
    return EHD_OK;
}

static int ehd_event_epoll_add(ehd_event_t * ev, int event, uint flags){
    int op;
    ehd_connection_t * c;
    struct epoll_event ee;
    c = ev->data;

    if (ev->active) {
        op = EPOLL_CTL_MOD;
    } else {
        op = EPOLL_CTL_ADD;
        ev->active = 1;
    }

    ee.events = event | flags;
    ee.data.ptr = c;
    ev->events = ee.events;

    if (epoll_ctl(ep, op, c->fd, &ee) == -1) {
        perror("epoll_ctl(ehd_event_epoll_add)");
        return EHD_ERROR;
    }
    return EHD_OK;
}

static int ehd_event_epoll_del(ehd_event_t * ev, int event, uint flags){
    int op;
    uint32_t events;
    uint active;
    ehd_connection_t * c;
    struct epoll_event ee;
    c = ev->data;
    if (flags == EHD_CLOSE_EVENT) {
        return EHD_OK;
    }
    op = EPOLL_CTL_DEL;
    /*
    if (ev->active) {
        events = ev->events;
        events &= event;
        if (events == (EPOLLIN | EPOLLOUT)) {
            op = EPOLL_CTL_MOD;
            if (event == EPOLLIN)
                events = EPOLLOUT;
            else
                events = EPOLLIN;
            ee.events = events | flags;
            ee.data.ptr = c;
            ee.events = ee.events;
            active = 1;
        } else {
            op = EPOLL_CTL_DEL;
            ee.events = 0;
            ee.data.ptr = NULL;
            ev->events = 0;
            active = 0;
        }
    } else {
        op = EPOLL_CTL_DEL;
        ee.events = 0;
        ee.data.ptr = NULL;
        ev->events = 0;
        active = 0;
    }
    */
    if (epoll_ctl(ep, op, c->fd, NULL) == -1) {
        return EHD_ERROR;
    }
    ev->active = active;
    return EHD_OK;
}

int ehd_event_epoll_process(ehd_cycle_t *cycle) {
    int events;
    size_t n;
    int i;
    int err;
    ehd_connection_t *c;
    ehd_event_t *ev;
    struct epoll_event *ee;

    ehd_event_enable_accept(cycle);

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    for( ; ; ) {
        /*
         * now epoll_wait timeout is -1 means block indefinitely
         * because the timer has not been implemented yet
         * */
        events = epoll_wait(ep, event_list, nevents, -1);
        if (events == 0) {
            continue;
        } else if (events == -1){
            err = errno;
        }

        for(i = 0;i < events;i++) {
            ee = &event_list[i];
            c = ee->data.ptr;
            ev = c->event;
            if (c->fd == -1) {
                continue;
            }
            if (ee->events & EPOLLOUT) {
                ev->wready = 1;
                if (ehd_threaded) {

                } else {
                    c->write_handler(c);
                }
            }
            if (ee->events & EPOLLIN) {
                ev->rready = 1;
                if (ehd_threaded) {

                } else {
                    c->read_handler(c);
                }
            }

        }
    }
#pragma clang diagnostic pop

}


