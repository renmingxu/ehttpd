//
// Created by renmiehdu on 12/21/17.
//

#include <ehd_config.h>
#include <ehd_core.h>
#include <ehd_http.h>


static void ehd_http_init_request(ehd_connection_t * c);


static void ehd_http_process_request_line(ehd_connection_t *c) ;

static ssize_t  ehd_http_read_request_header(ehd_http_request_t *r);


static void ehd_http_process_request_headers(ehd_connection_t *c);

static int ehd_http_process_request_header(ehd_http_request_t *r);

static void ehd_http_block_read(ehd_connection_t * c) ;

ehd_http_header_t  ehd_http_headers_in[] = {
        { ehd_string("Host"), offsetof(ehd_http_headers_in_t, host) },
        { ehd_string("Connection"), offsetof(ehd_http_headers_in_t, connection) },
        { ehd_string("If-Modified-Since"),
          offsetof(ehd_http_headers_in_t, if_modified_since) },
        { ehd_string("User-Agent"), offsetof(ehd_http_headers_in_t, user_agent) },
        { ehd_string("Referer"), offsetof(ehd_http_headers_in_t, referer) },
        { ehd_string("Content-Length"),
          offsetof(ehd_http_headers_in_t, content_length) },

        { ehd_string("Range"), offsetof(ehd_http_headers_in_t, range) },

        { ehd_string("Authorization"),
          offsetof(ehd_http_headers_in_t, authorization) },

        { ehd_string("Keep-Alive"), offsetof(ehd_http_headers_in_t, keep_alive) },

        { ehd_null_string, 0 }
};

void ehd_http_init_connection(ehd_connection_t *c) {
    ehd_event_t  *ev;

    ev = c->event;
    c->read_handler = ehd_http_init_request;
    c->write_handler = ehd_http_empty_handler;

    if (!ev->closed) {
        ehd_http_init_request(c);
    }
}

static void ehd_http_init_request(ehd_connection_t * c) {
    ehd_event_t * ev;
    ehd_http_connection_t * hc;
    ehd_http_request_t * r;
    ehd_pool_t * cpool, * rpool;

    hc = c->data;
    cpool = c->pool;
    if (!hc) {
        if (!(hc = ehd_pcalloc(cpool, sizeof(ehd_http_connection_t)))) {
            ehd_http_close_connection(c);
            return;
        }
    }
    r = hc->request;
    if (r) {
        bzero(r,sizeof(ehd_http_request_t));
    } else {
        if (!(r = ehd_pcalloc(c->pool, sizeof(ehd_http_request_t)))) {
            ehd_http_close_connection(c);
            return;
        }
        hc->request = r;
    }
    r->http_state = EHD_HTTP_INITING_REQUEST_STATE;
    c->data = r;
    r->http_connection = hc;
    r->conf = ehd_cycle->conf;
    if (c->buf == NULL) {
        c->buf = ehd_create_temp_buf(cpool, EHD_DEFAULT_BUF_SIZE);
        if (c->buf == NULL) {
            ehd_http_close_connection(c);
            return;
        }
    }
    if (r->header_in_buf == NULL) {
        r->header_in_buf = c->buf;
    }
    if (!(r->pool = ehd_create_pool_default_size())) {
        ehd_http_close_connection(c);
        return;
    }
    rpool = r->pool;
    if (ehd_list_init(&r->headers_out.headers, rpool, 20, sizeof(ehd_table_data_t)) == EHD_ERROR) {
        ehd_http_close_request(r);
        ehd_http_close_connection(c);
        return;
    }
    r->connection = c;
    r->headers_in.content_length_n = -1;
    r->headers_out.content_length_n = -1;
    r->headers_out.last_moddified_time = -1;
    r->http_state = EHD_HTTP_READING_REQUEST_STATE;
    c->read_handler = ehd_http_process_request_line;
    c->read_handler(c);
}

void ehd_http_close_request(ehd_http_request_t *r) {

    if (r->pool == NULL) {
        return;
    }
    ehd_destroy_pool(r->pool);
}


static void ehd_http_process_request_line(ehd_connection_t *c) {
    printf("process request line\n");
    ehd_http_request_t * r;
    ehd_event_t * ev;

    r = c->data;
    ev = c->event;

    ssize_t n;
    int i;
    i = EHD_AGAIN;
    for( ;; ) {
        if (i == EHD_AGAIN) {
            n = ehd_http_read_request_header(r);
            printf("recv %ld\n", n);
            if (n == EHD_AGAIN) {
                return;
            } else if (n == EHD_ERROR) {
                /* handler error */
//                ehd_http_close_request(r);
//                ehd_http_close_connection(c);
                return;
            }
        }
        i = ehd_http_parse_request_line(r, r->header_in_buf);
        if (i == EHD_OK) {
            /*
             * request line has been parsed successfully
             * */

            /* copy unparsed_uri */

            r->unparsed_uri.len = r->uri_end - r->uri_start;
            r->unparsed_uri.data = ehd_palloc(r->pool, r->unparsed_uri.len + 1);
            if (r->unparsed_uri.data == NULL) {
                ehd_http_close_request(r);
                ehd_http_close_connection(c);
                return;
            }

            ehd_cpystrn(r->unparsed_uri.data, r->uri_start,
                        r->unparsed_uri.len + 1);

            /* copy URI */

            if (r->args_start) {
                r->uri.len = r->args_start - 1 - r->uri_start;
            } else {
                r->uri.len = r->uri_end - r->uri_start;
            }

            if (!(r->uri.data = ehd_palloc(r->pool, r->uri.len + 1))) {
                ehd_http_close_request(r);
                ehd_http_close_connection(c);
                return;
            }

            if (r->complex_uri) {
                i = ehd_http_parse_complex_uri(r);

                if (i == EHD_HTTP_INTERNAL_SERVER_ERROR) {
                    ehd_http_close_request(r);
                    ehd_http_close_connection(c);
                    return;
                }

                if (i != EHD_OK) {
                    r->request_line.len = r->request_end - r->request_start;
                    r->request_line.data = r->request_start;

                    ehd_http_close_request(r);
                    ehd_http_close_connection(c);
                    return;
                }
            } else {
                ehd_cpystrn(r->uri.data, r->uri_start, r->uri.len + 1);
            }
            r->request_line.len = r->request_end - r->request_start;
            r->request_line.data = r->request_start;
            r->request_line.data[r->request_line.len] = '\0';

            if (r->method == 0) {
                r->method_name.len = r->method_end - r->request_start + 1;
                r->method_name.data = r->request_line.data;
            }

            if (r->uri_ext) {

                /* copy URI extention */

                if (r->args_start) {
                    r->exten.len = r->args_start - 1 - r->uri_ext;
                } else {
                    r->exten.len = r->uri_end - r->uri_ext;
                }

                if (!(r->exten.data = ehd_palloc(r->pool, r->exten.len + 1))) {
                    ehd_http_close_request(r);
                    ehd_http_close_connection(c);
                    return;
                }

                ehd_cpystrn(r->exten.data, r->uri_ext, r->exten.len + 1);
            }
            r->request_line.len = r->request_end - r->request_start;
            r->request_line.data = r->request_start;
            r->request_line.data[r->request_line.len] = '\0';

            if (r->method == 0) {
                r->method_name.len = r->method_end - r->request_start + 1;
                r->method_name.data = r->request_line.data;
            }

            if (r->uri_ext) {

                /* copy URI extention */

                if (r->args_start) {
                    r->exten.len = r->args_start - 1 - r->uri_ext;
                } else {
                    r->exten.len = r->uri_end - r->uri_ext;
                }

                if (!(r->exten.data = ehd_palloc(r->pool, r->exten.len + 1))) {
                    ehd_http_close_request(r);
                    ehd_http_close_connection(c);
                    return;
                }

                ehd_cpystrn(r->exten.data, r->uri_ext, r->exten.len + 1);
            }

            if (ehd_list_init(&r->headers_in.headers, r->pool, 20,
                              sizeof(ehd_table_data_t)) == EHD_ERROR)
            {
                ehd_http_close_request(r);
                ehd_http_close_connection(c);
                return;
            }


            if (ehd_array_init(&r->headers_in.cookies, r->pool, 5,
                               sizeof(ehd_table_data_t *)) == EHD_ERROR)
            {
                ehd_http_close_request(r);
                ehd_http_close_connection(c);
                return;
            }

            c->read_handler = ehd_http_process_request_headers;
            ehd_http_process_request_headers(c);

            return;

        } else if (i != EHD_AGAIN) {
            /* error */
            ehd_http_close_request(r);
            ehd_http_close_connection(c);
            return;
        }
        if (r->header_in_buf->pos == r->header_in_buf->end) {
            ehd_http_close_request(r);
            ehd_http_close_connection(c);
            return;
        }
    }
}

static void ehd_http_process_request_headers(ehd_connection_t *c){
    printf("process request headers\n");

    int rc, i;
    ssize_t  n;
    ehd_event_t *ev;
    ehd_table_data_t *h,**cookie;
    ehd_http_request_t * r;

    ev = c->event;
    r = c->data;

    rc = EHD_AGAIN;

    for(;;) {
        if (rc == EHD_AGAIN) {
            if (r->header_in_buf->pos == r->header_in_buf->end) {
                ehd_http_close_request(r);
                ehd_http_close_connection(c);
                return;
            }
            n = ehd_http_read_request_header(r);
            if (n == EHD_AGAIN) {
                return;
            } else if(n == EHD_ERROR) {
                ehd_http_close_request(r);
                ehd_http_close_connection(c);
                return;
            }
        }
        rc = ehd_http_parse_header_line(r, r->header_in_buf);

        if (rc == EHD_OK) {
            /* one header line of a request has been parsed successfully */

            r->headers_n++;
            if (!(h = ehd_list_push(&r->headers_in.headers))) {
                ehd_http_close_request(r);
                ehd_http_close_connection(c);
                return;
            }

            h->key.len = r->header_name_end - r->header_name_start;
            h->key.data = r->header_name_start;
            h->key.data[h->key.len] = '\0';

            h->value.len = r->header_end - r->header_start;
            h->value.data = r->header_start;
            h->value.data[h->value.len] = '\0';

            if (h->key.len == sizeof("Cookie") - 1
                && strcasecmp((const char *) h->key.data, "Cookie") == 0)
            {
                if (!(cookie = ehd_array_push(&r->headers_in.cookies))) {
                    ehd_http_close_request(r);
                    ehd_http_close_connection(c);
                    return;
                }

                *cookie = h;

            } else {

                for (i = 0; ehd_http_headers_in[i].name.len != 0; i++) {
                    if (ehd_http_headers_in[i].name.len != h->key.len) {
                        continue;
                    }

                    if (strcasecmp((const char *) ehd_http_headers_in[i].name.data,
                                   (const char *) h->key.data) == 0)
                    {
                        *((ehd_table_data_t **) ((char *) &r->headers_in
                                                + ehd_http_headers_in[i].offset)) = h;
                        break;
                    }
                }
            }
            continue;
        } else if (rc == EHD_HTTP_PARSE_HEADER_DONE) {
            r->http_state = EHD_HTTP_PROCESS_REQUEST_STATE;

            rc = ehd_http_process_request_header(r);

            if (rc != EHD_OK) {
                ehd_http_close_request(r);
                ehd_http_close_connection(c);
                return;
            }

            c->read_handler = ehd_http_block_read;

            ehd_http_handler(r);

            return;
        } else if (rc != EHD_AGAIN) {
            ehd_http_close_request(r);
            ehd_http_close_connection(c);
            return;
        }

    }

}
static int ehd_http_process_request_header(ehd_http_request_t *r) {
    return EHD_OK;
}


static void ehd_http_block_read(ehd_connection_t * c) {
    printf("ehd_http_block_read\n");
    ehd_http_request_t * r;
    if (ehd_del_event(c->event, EHD_READ_EVENT, 0) == EHD_ERROR) {
        r = c->data;
        ehd_http_close_request(r);
        ehd_http_close_connection(c);
    }
}



static ssize_t ehd_http_read_request_header(ehd_http_request_t *r) {
    ssize_t n;
    ehd_event_t *ev;
    ev = r->connection->event;
    n = r->header_in_buf->last - r->header_in_buf->pos;
    if (n > 0) {
        return n;
    }
    if (!ev->rready) {
        return EHD_AGAIN;
    }
    n = ehd_recv(r->connection, r->header_in_buf->last, r->header_in_buf->end - r->header_in_buf->last);

    if (n == EHD_AGAIN) {
        return EHD_AGAIN;
    }
    if (n == 0 || n == EHD_ERROR) {
        ehd_http_close_request(r);
        ehd_http_close_connection(r->connection);
        return EHD_ERROR;
    }

    r->header_in_buf->last += n;
    return n;
}

void ehd_http_keep_alive(ehd_http_request_t *r) {
    ehd_http_connection_t *hc;
    ehd_connection_t * c;
    ehd_buf_t * b;
    hc = r->http_connection;
    hc->request = NULL;
    c = r->connection;
    c->data = hc;
    c->read_handler = ehd_http_init_request;
    b = r->header_in_buf;
    int i;
    if (b->pos < b->last) {
        for(i = 0;i < b->last - b->pos;i++) {
            b->start[i] = b->pos[i];
        }
        b->pos = b->start;
        b->last = b->start + i;
    } else {
        b->pos = b->start;
        b->last = b->start;
    }

    ehd_http_close_request(r);
    c->write_handler = ehd_http_empty_handler;

    if (ehd_add_event(c->event,EHD_READ_EVENT,EPOLLET) == EHD_ERROR) {
        ehd_http_close_connection(c);
    }
}




