//
// Created by renmingxu on 12/25/17.
//

#include <ehd_config.h>
#include <ehd_core.h>


int ehd_open_listen_socket(ehd_cycle_t cycle) {
    return 0;
}

int ehd_listening_init(ehd_cycle_t *cycle, ehd_conf_t *conf) {
    ehd_pool_t * pool;
    ehd_list_t * addr_list;
    ehd_list_t * port_list;
    ehd_list_node_t *addr_node;
    ehd_list_node_t *port_node;
    ehd_str_t *addr;
    uint16_t port;
    ehd_listening_t *listening;
    struct sockaddr_in *sockaddr;
    pool = cycle->pool;
    int n;
    addr_list = &conf->addrs_str;
    port_list = &conf->ports_int;
    for(addr_node = &addr_list->head, port_node= &port_list->head;
        addr_node != NULL && port_node != NULL;
        addr_node = addr_node->next, port_node = port_node->next)
    {
        for(n = 0;n < addr_node->ndata && n < port_node->ndata;n++) {
            addr = addr_node->data + n * addr_list->size;
            port = *(uint16_t *)(port_node->data + n * port_list->size);
            listening = ehd_array_push(&cycle->listening);
            sockaddr = ehd_palloc(pool, sizeof(struct sockaddr_in));
            listening->sockaddr = (struct sockaddr*)sockaddr;
            listening->socklen = sizeof(struct sockaddr_in);
            listening->family = AF_INET;
            listening->type = SOCK_STREAM;
            listening->protocol = 0;
            listening->fd = -1;
            listening->backlog = 0;
            listening->pool_size = 16 * 1024;
            bzero(sockaddr, sizeof(struct sockaddr_in));
            sockaddr->sin_family = AF_INET;
            sockaddr->sin_port = ntohs(port);
            inet_pton(sockaddr->sin_family, (char *) addr->data, &sockaddr->sin_addr);
            printf("addr: %s ", addr->data);
            printf("port: %d \n", port);
        }
    }


}

int ehd_open_listening_socket(ehd_cycle_t *cycle) {
    ehd_listening_t *listenings = cycle->listening.data;
    ehd_listening_t * lis;
    int sockfd, reuse_int, err, tries, failed;

    reuse_int = 1;
    failed = 0;

    int i;
    for(tries = 5;tries;tries--) {
        failed = 0;

        for (i = 0;i < cycle->listening.ndata;i++) {
            lis = listenings + i;
            if (lis->fd != -1) {
                continue;
            }

            sockfd = socket(lis->family, lis->type, lis->protocol);

            if (sockfd == -1) {
                return EHD_ERROR;
            }

            if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse_int, sizeof(int)) == -1) {
                return EHD_ERROR;
            }

            if (ehd_setnonblocking(sockfd) == -1) {
                return EHD_ERROR;
            }

            if (bind(sockfd, lis->sockaddr, lis->socklen) == -1) {
                err = errno;

                if (err == EADDRINUSE)
                    return EHD_ERROR;
                failed = 1;
                continue;
            }

            if (listen(sockfd, lis->backlog) == -1) {
                return EHD_ERROR;
            }
            lis->fd = sockfd;
        }
        if(!failed) {
            break;
        }
        usleep(500);
    }
    if (failed) {
        return EHD_ERROR;
    }
    return EHD_OK;

}

void ehd_close_connection(ehd_connection_t *c) {
    int fd;

    if (c->pool == NULL) {
        /* this error does not need to be handled */
        return;
    }
    if (ehd_del_event) {
        ehd_del_event(c->event, 0, EHD_CLOSE_EVENT);
    }
    c->event->closed = 1;
    fd = c->fd;
    c->fd = -1;
    c->data = NULL;
    ehd_destroy_pool(c->pool);
    if  (close(fd) == -1) {
        /* this error does not need to be handled */
        return;
    }
}
