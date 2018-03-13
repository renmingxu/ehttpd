//
// Created by renmingxu on 12/26/17.
//

#ifndef EHTTPD_EHD_SOCKET_H
#define EHTTPD_EHD_SOCKET_H

#include <ehd_config.h>
#include <ehd_core.h>

int ehd_setnonblocking(int fd);
int ehd_setblocking(int fd);

ssize_t ehd_recv(ehd_connection_t * c, u_char * buf, size_t size);
ssize_t ehd_send(ehd_connection_t * c, u_char * buf, size_t size);

#endif //EHTTPD_EHD_SOCKET_H
