//
// Created by renmingxu on 12/21/17.
//

#ifndef EHTTPD_EHD_CONNECTION_H
#define EHTTPD_EHD_CONNECTION_H

#include <ehd_config.h>
#include <ehd_core.h>


struct ehd_connection_s {
    /*
     * ehd_http_request_t when process a http request
     * or ehd_http_connection when keep alive
     * */
    void * data;
    int fd;

    ehd_event_t * event; /* we use only one ehd_event_t for epoll_impl */

    /*
     * event_impl.process will call c->read_handler(c)
     * when EHD_READ_EVENT is avialable
     * call c->write_handler(c)
     * when EHD_WRITE_EVENT is avialable
     * */
    void (* read_handler)(ehd_connection_t *);
    void (* write_handler)(ehd_connection_t *);

    ehd_listening_t * listening;

    ehd_pool_t *  pool;

    ehd_buf_t * buf;

    ehd_sockaddr_t sockaddr;
    socklen_t socklen;

};

struct ehd_listening_s {
    int fd;
    struct sockaddr * sockaddr;
    socklen_t socklen;
    size_t pool_size;
    void (*handler) (ehd_connection_t *c);
    int family;
    int type;
    int protocol;
    int backlog;
};

int ehd_listening_init(ehd_cycle_t *cycle, ehd_conf_t * conf);

int ehd_open_listening_socket(ehd_cycle_t *cycle);

void ehd_close_connection(ehd_connection_t *c);


#endif //EHTTPD_EHD_CONNECTION_H
