//
// Created by renmingxu on 12/21/17.
//

#ifndef EHTTPD_EHD_EVENT_H
#define EHTTPD_EHD_EVENT_H

#include <ehd_config.h>
#include <ehd_core.h>

typedef struct ehd_event_s ehd_event_t;
typedef union ehd_sockaddr_u ehd_sockaddr_t;
typedef struct ehd_event_impl_s ehd_event_impl_t;

extern ehd_event_impl_t ehd_event_impl_epoll;

extern ehd_event_impl_t ehd_event_impl;

extern struct ehd_event_impl_funp_s ehd_event_funps;

#define ehd_add_event ehd_event_funps.add
#define ehd_del_event ehd_event_funps.del
#define ehd_mod_event ehd_event_funps.mod
#define ehd_process_event ehd_event_funps.process


#define EHD_READ_EVENT EPOLLIN
#define EHD_WRITE_EVENT EPOLLOUT

#define EHD_CLOSE_EVENT 1

struct ehd_event_s {
    void  *data; /* now it is always ehd_connection */
    uint32_t events;
    unsigned closed:1;
    unsigned accept:1;
    unsigned active:1;
    unsigned rready:1;
    unsigned wready:1;
    unsigned eof:1;
    unsigned disalbed:1;
    unsigned error:1;
};

union ehd_sockaddr_u {
    struct sockaddr sockaddr;
    struct sockaddr_in sockaddr_in;
};

struct ehd_event_impl_funp_s {
    int (*process) (ehd_cycle_t *cycle);
    int (*add) (ehd_event_t *ev, int event, uint flags);
    int (*del) (ehd_event_t *ev, int event, uint flags);
    int (*mod) (ehd_event_t *ev, int event, uint flags);
    int (*init) (ehd_cycle_t * cycle);
    int (*close) (ehd_cycle_t * cycl);
};

struct ehd_event_impl_s {
    ehd_str_t * name;
    struct ehd_event_impl_funp_s funp;
};



int ehd_event_init(ehd_cycle_t *cycle);

void ehd_event_accept_handler(ehd_connection_t *c);
int ehd_event_enable_accept(ehd_cycle_t * cycle);
int ehd_event_disable_accept(ehd_cycle_t * cycle);


void ehd_event_echo_read(ehd_connection_t *c);
void ehd_event_echo_write(ehd_connection_t *c);


#endif //EHTTPD_EHD_EVENT_H
