//
// Created by renmingxu on 12/26/17.
//

#include <ehd_config.h>
#include <ehd_core.h>

int ehd_setnonblocking(int fd) {

    /*
    int opts = 0;
    if ((opts = fcntl(fd, F_GETFL, 0)) < 0) {
        return EHD_ERROR;
    }
    if (fcntl(fd, F_SETFL, opts | O_NONBLOCK) < 0) {
        return EHD_ERROR;
    }
    return EHD_OK;
     */

    int  nb;

    nb = 1;

    return ioctl(fd, FIONBIO, &nb);
}
int ehd_setblocking(int fd) {
    /*
    int opts = 0;
    if ((opts = fcntl(fd, F_GETFL, 0)) < 0) {
        return EHD_ERROR;
    }
    if (fcntl(fd, F_SETFL, opts | O_NONBLOCK) < 0) {
        return EHD_ERROR;
    }
    return EHD_OK;
     */
    int  nb;

    nb = 0;

    return ioctl(fd, FIONBIO, &nb);
}

ssize_t ehd_recv(ehd_connection_t *c, u_char *buf, size_t size) {
    ssize_t n;
    ehd_event_t *ev;
    int err;

    ev = c->event;

    do {
        n = recv(c->fd, buf, size, 0);
        if (n == 0) {
            ev->rready = 0;
            ev->eof = 1;
            return n;
        } else if (n > 0)  {
            if ((size_t) n < size) {
                ev->rready = 0;
            }
            return n;
        }
        err = errno;
        if (err == EAGAIN) {
            ev->rready = 0;
            n = EHD_AGAIN;
            return n;
        } if (err == EINTR){
            n = EHD_AGAIN;
        } else {
            n = EHD_ERROR;
            break;
        }
    } while(err == EINTR);

    ev->rready = 0;
    ev->error = 1;
    return n;
}

ssize_t ehd_send(ehd_connection_t *c, u_char *buf, size_t size) {
    ssize_t n;
    ehd_event_t *ev;
    int err;

    ev = c->event;

    do {
        n = send(c->fd, buf, size, 0);
        if (n > 0)  {
            if ((size_t) n < size) {
                ev->wready = 0;
            }
            return n;
        }
        if (n == 0) {
            ev->wready = 0;
            return n;
        }
        err = errno;
        if (err == EAGAIN) {
            ev->wready = 0;
            n = EHD_AGAIN;
            return n;
        } if (err == EINTR){
            n = EHD_AGAIN;
        } else {
            n = EHD_ERROR;
            break;
        }
    } while(err == EINTR);

    ev->rready = 0;
    ev->error = 1;
    return n;
}
