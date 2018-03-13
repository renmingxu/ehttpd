//
// Created by renmingxu on 12/31/17.
//

#include <ehd_config.h>
#include <ehd_core.h>


void ehd_event_echo_read(ehd_connection_t *c) {
    ehd_pool_t * pool;
    int fd;
    ehd_buf_t *buf;
    int err;
    ssize_t n;

    pool = c->pool;
    fd = c->fd;
    if (c->buf == NULL) {
        buf = c->buf = ehd_create_temp_buf(pool, 4096);
    } else {
        buf = c->buf;
        buf->pos = buf->start;
        buf->last = buf->start;
        bzero(buf->start, buf->end - buf->start);
    }

    for(;;) {
        n = recv(fd, buf->last, buf->end - buf->last, 0);
        if (n == -1) {
            err = errno;
            if (err == EAGAIN) {
                break;
            }
            ehd_close_connection(c);
            return;
        } else if(n == 0) {
            ehd_close_connection(c);
            return;
        } else {
            buf->last += n;
            if (buf->last == buf->end) {
                break;
            }
        }
    }
//    printf("recv len %ld from %d\n", buf->last - buf->pos, fd);
    ehd_mod_event(c->event, EHD_WRITE_EVENT, EPOLLET);
}


void ehd_event_echo_write(ehd_connection_t *c) {
    ehd_buf_t * buf;
    buf = c->buf;
    ssize_t n;
    int err;
    for(;;) {
        n = send(c->fd, buf->pos, buf->last - buf->pos, 0);
        if (n == -1) {
            err = errno;
            if (err == EAGAIN) {
                break;
            }
            ehd_close_connection(c);
            return;
        } else if(n == 0) {
            ehd_close_connection(c);
            return;
        } else {
            buf->pos += n;
            if (buf->pos == buf->last) {
                break;
            }
        }
    }
    ehd_mod_event(c->event, EHD_READ_EVENT, EPOLLET);
}

