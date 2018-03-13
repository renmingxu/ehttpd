//
// Created by renmingxu on 1/2/18.
//

#ifndef EHTTPD_EHD_HTTP_H
#define EHTTPD_EHD_HTTP_H

#include <ehd_config.h>
#include <ehd_core.h>
#include <ehd_http_request.h>
#include <ehd_http_core.h>

typedef struct ehd_http_request_s ehd_http_request_t;

int ehd_http_init(ehd_cycle_t * cycle);

void ehd_http_init_connection(ehd_connection_t *c);

int ehd_http_parse_request_line(ehd_http_request_t * r, ehd_buf_t * buf);

int ehd_http_parse_complex_uri(ehd_http_request_t *r);

int ehd_http_parse_header_line(ehd_http_request_t *r, ehd_buf_t *b);

void ehd_http_keep_alive(ehd_http_request_t *r);

void ehd_http_close_request(ehd_http_request_t *r);

void ehd_http_close_connection(ehd_connection_t *c);

void ehd_http_handler(ehd_http_request_t * r);



void ehd_http_empty_handler(ehd_connection_t *c);

#endif //EHTTPD_EHD_HTTP_H
