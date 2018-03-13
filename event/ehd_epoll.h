//
// Created by renmingxu on 12/31/17.
//

#ifndef EHTTPD_EHD_EPOLL_H
#define EHTTPD_EHD_EPOLL_H
#include <ehd_config.h>
#include <ehd_core.h>

static ehd_str_t epoll_name = ehd_string("epoll");
static int      ep = -1;
static struct   epoll_event *event_list;
static u_int    nevents = 0;

struct ehd_event_impl_funp_s ehd_event_funps;

static int ehd_event_epoll_init(ehd_cycle_t * cycle);

static int ehd_event_epoll_close(ehd_cycle_t * cycle);

static int ehd_event_epoll_add(ehd_event_t * ev, int event, uint flags);

static int ehd_event_epoll_mod(ehd_event_t * ev, int event, uint flags);

static int ehd_event_epoll_del(ehd_event_t * ev, int event, uint flags);

static int ehd_event_epoll_process(ehd_cycle_t *cycle);

struct ehd_event_impl_s ehd_event_impl_epoll =  {
        &epoll_name,
        {
                ehd_event_epoll_process,
                ehd_event_epoll_add,
                ehd_event_epoll_del,
                ehd_event_epoll_mod,
                ehd_event_epoll_init,
                ehd_event_epoll_close,
        },
};



#endif //EHTTPD_EHD_EPOLL_H
