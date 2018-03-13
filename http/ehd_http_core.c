//
// Created by renmingxu on 1/3/18.
//

#include <ehd_config.h>
#include <ehd_core.h>
#include <ehd_http.h>
#include "ehd_http_core.h"


static ehd_str_t mime[][2] = {
        {ehd_string("html"), ehd_string("text/html")},
        {ehd_string("png"),ehd_string("image/png")},
        {ehd_string("jpg"),ehd_string("image/jpeg")},
        {ehd_string("js"),ehd_string("application/javascript")},
        {ehd_string("ttf"),ehd_string("font/ttf")},
        {ehd_string("otf"),ehd_string("font/otf")},
        {ehd_string("svg"),ehd_string("image/svg+xml")},
        {ehd_string("woff"),ehd_string("font/woff")},
        {ehd_string("woff2"),ehd_string("font/woff2")},
        {ehd_string("css"),ehd_string("text/css")},

};

static u_char error_tail[] =
        "<hr><center>" EHD_VERSION "</center>" CRLF
                "</body>" CRLF
                "</html>" CRLF
;
static ehd_str_t error_tail_t = ehd_string(error_tail);

static char error_301_page[] =
        "<html>" CRLF
                "<head><title>301 Moved Permanently</title></head>" CRLF
                "<body bgcolor=\"white\">" CRLF
                "<center><h1>301 Moved Permanently</h1></center>" CRLF
;


static char error_302_page[] =
        "<html>" CRLF
                "<head><title>302 Found</title></head>" CRLF
                "<body bgcolor=\"white\">" CRLF
                "<center><h1>302 Found</h1></center>" CRLF
;


static char error_400_page[] =
        "<html>" CRLF
                "<head><title>400 Bad Request</title></head>" CRLF
                "<body bgcolor=\"white\">" CRLF
                "<center><h1>400 Bad Request</h1></center>" CRLF
;


static char error_403_page[] =
        "<html>" CRLF
                "<head><title>403 Forbidden</title></head>" CRLF
                "<body bgcolor=\"white\">" CRLF
                "<center><h1>403 Forbidden</h1></center>" CRLF
;


static char error_404_page[] =
        "<html>" CRLF
                "<head><title>404 Not Found</title></head>" CRLF
                "<body bgcolor=\"white\">" CRLF
                "<center><h1>404 Not Found</h1></center>" CRLF
;


static char error_405_page[] =
        "<html>" CRLF
                "<head><title>405 Not Allowed</title></head>" CRLF
                "<body bgcolor=\"white\">" CRLF
                "<center><h1>405 Not Allowed</h1></center>" CRLF
;


static char error_408_page[] =
        "<html>" CRLF
                "<head><title>408 Request Time-out</title></head>" CRLF
                "<body bgcolor=\"white\">" CRLF
                "<center><h1>408 Request Time-out</h1></center>" CRLF
;


static char error_413_page[] =
        "<html>" CRLF
                "<head><title>413 Request Entity Too Large</title></head>" CRLF
                "<body bgcolor=\"white\">" CRLF
                "<center><h1>413 Request Entity Too Large</h1></center>" CRLF
;


static char error_414_page[] =
        "<html>" CRLF
                "<head><title>414 Request-URI Too Large</title></head>" CRLF
                "<body bgcolor=\"white\">" CRLF
                "<center><h1>414 Request-URI Too Large</h1></center>" CRLF
;


static char error_416_page[] =
        "<html>" CRLF
                "<head><title>416 Requested Range Not Satisfiable</title></head>" CRLF
                "<body bgcolor=\"white\">" CRLF
                "<center><h1>416 Requested Range Not Satisfiable</h1></center>" CRLF
;


static char error_497_page[] =
        "<html>" CRLF
                "<head><title>400 The plain HTTP request was sent to HTTPS port</title></head>"
                CRLF
                "<body bgcolor=\"white\">" CRLF
                "<center><h1>400 Bad Request</h1></center>" CRLF
                "<center>The plain HTTP request was sent to HTTPS port</center>" CRLF
;


static char error_500_page[] =
        "<html>" CRLF
                "<head><title>500 Internal Server Error</title></head>" CRLF
                "<body bgcolor=\"white\">" CRLF
                "<center><h1>500 Internal Server Error</h1></center>" CRLF
;


static char error_501_page[] =
        "<html>" CRLF
                "<head><title>501 Method Not Implemented</title></head>" CRLF
                "<body bgcolor=\"white\">" CRLF
                "<center><h1>500 Method Not Implemented</h1></center>" CRLF
;


static char error_502_page[] =
        "<html>" CRLF
                "<head><title>502 Bad Gateway</title></head>" CRLF
                "<body bgcolor=\"white\">" CRLF
                "<center><h1>502 Bad Gateway</h1></center>" CRLF
;


static char error_503_page[] =
        "<html>" CRLF
                "<head><title>503 Service Temporarily Unavailable</title></head>" CRLF
                "<body bgcolor=\"white\">" CRLF
                "<center><h1>503 Service Temporarily Unavailable</h1></center>" CRLF
;


static char error_504_page[] =
        "<html>" CRLF
                "<head><title>504 Gateway Time-out</title></head>" CRLF
                "<body bgcolor=\"white\">" CRLF
                "<center><h1>504 Gateway Time-out</h1></center>" CRLF
;


static ehd_str_t error_pages[] = {
        /* ehd_null_string, */          /* 300 */
        ehd_string(error_301_page),
        ehd_string(error_302_page),
        ehd_null_string,             /* 303 */

        ehd_string(error_400_page),
        ehd_null_string,             /* 401 */
        ehd_null_string,             /* 402 */
        ehd_string(error_403_page),
        ehd_string(error_404_page),
        ehd_string(error_405_page),
        ehd_null_string,             /* 406 */
        ehd_null_string,             /* 407 */
        ehd_string(error_408_page),
        ehd_null_string,             /* 409 */
        ehd_null_string,             /* 410 */
        ehd_null_string,             /* 411 */
        ehd_null_string,             /* 412 */
        ehd_string(error_413_page),
        ehd_string(error_414_page),
        ehd_null_string,             /* 415 */
        ehd_string(error_416_page),

        ehd_string(error_497_page),  /* 497, http to https */
        ehd_string(error_404_page),  /* 498, invalid host name */
        ehd_null_string,             /* 499, client closed connection */

        ehd_string(error_500_page),
        ehd_string(error_501_page),
        ehd_string(error_502_page),
        ehd_string(error_503_page),
        ehd_string(error_504_page)
};

static void ehd_http_writer(ehd_connection_t *c);

void ehd_http_file_writer_handler(ehd_http_request_t *r);

void ehd_http_special_response_handler(ehd_http_request_t *r, int error) ;

int ehd_http_response_gen_header(ehd_http_request_t *r) ;

void ehd_http_handler(ehd_http_request_t * r) {
    r->state = 0;
    if (r->method == EHD_HTTP_GET) {
        ehd_http_file_writer_handler(r);
    } else {
        ehd_http_special_response_handler(r, EHD_HTTP_NOT_IMPLEMENTED);
    }
}

void ehd_http_file_writer_handler(ehd_http_request_t *r) {
    printf("writer init for uri: %s\n", r->uri.data);
    ehd_conf_t *conf;
    ehd_event_t * ev;
    ehd_connection_t *c;
    struct stat stbuf;
    int n,i,j,k,tmp_flag;
    ehd_file_t file;
    ehd_str_t filename, *content_type, *uri;
    ehd_pool_t *pool;
    u_char * f;
    ehd_table_data_t *t;
    ehd_http_headers_out_t *h;
    ehd_file_t * open_file;
    void (*write_handler)(ehd_connection_t*);

    c = r->connection;
    ev = c->event;
    pool = r->pool;
    conf = r->conf;
    uri = &r->uri;
    h = &r->headers_out;

    filename.data = ehd_pcalloc(pool, conf->root.len + uri->len + conf->index.len + 2);
    filename.len = 0;

    memcpy(filename.data, conf->root.data, conf->root.len);
    memcpy(filename.data + conf->root.len, uri->data, uri->len);
    filename.len = conf->root.len + uri->len;
    f = filename.data;

    if ((n = stat((const char *) f, &stbuf)) == -1){
        ehd_http_special_response_handler(r,EHD_HTTP_NOT_FOUND);
        return;
    }
    if (S_ISDIR(stbuf.st_mode)) {
        if (uri->data[uri->len - 1] != '/') {
            t = ehd_list_push(&r->headers_out.headers);
            t->key.data = (u_char *) "Location";
            t->key.len = sizeof("Location") - 1;
            memcpy(filename.data, uri->data, uri->len);
            filename.data[uri->len] = '/';
            filename.len = uri->len + 1;
            t->value = filename;
            ehd_http_special_response_handler(r,EHD_HTTP_MOVED_TEMPORARILY);
            return;
        } else {
            memcpy(filename.data + filename.len, conf->index.data, conf->index.len);
            filename.len += conf->index.len;
            f = filename.data;

            if ((n = stat((const char *) f, &stbuf)) == -1){
                ehd_http_special_response_handler(r,EHD_HTTP_FORBIDDEN);
                return;
            }
        }

    } else if (!S_ISREG(stbuf.st_mode)) {
        ehd_http_special_response_handler(r, EHD_HTTP_NOT_FOUND);
        return;
    }
    n = sizeof(mime) / sizeof(ehd_str_t) / 2;
    content_type = NULL;
    for (i = 0;i < n;i++) {
        tmp_flag = 1;
        for(j = (int) mime[i][0].len; j; j--) {
            if (mime[i][0].data[j - 1] != uri->data[uri->len - mime[i][0].len + j - 1]) {
                tmp_flag = 0;
                break;
            }
        }
        if (tmp_flag) {
            content_type = &mime[i][1];
            break;
        }
    }
    if (content_type != NULL) {
        printf("%s\n", content_type->data);
        h->content_type = t = ehd_list_push(&h->headers);
        t->key.data = (u_char *) "Content-Type";
        t->key.len = sizeof("Content-Type") - 1;
        t->value = *content_type;
    }
    h->status = 200;
    open_file = ehd_pcalloc(pool,sizeof(ehd_file_t));
    ehd_file_open_rs(open_file, filename);
    h->content_length_n = open_file->size;
    ehd_http_response_gen_header(r);

    r->response.last->next = ehd_pcalloc(pool,sizeof(ehd_chain_t));
    r->response.last = r->response.last->next;
    r->response.last->buf = ehd_pcalloc(pool,sizeof(ehd_buf_t));
    r->response.last->next = NULL;
    r->response.last->buf->type = 1;
    r->response.last->buf->file = open_file;
    c->write_handler = ehd_http_writer;
    if (ehd_add_event(ev, EHD_WRITE_EVENT, EPOLLET) == EHD_ERROR) {
        ehd_http_close_request(r);
        ehd_http_close_connection(c);
    }
    ehd_http_writer(c);
}



void ehd_http_special_response_handler(ehd_http_request_t *r, int error) {
    int err;
    ehd_pool_t *pool;
    ehd_buf_t *b;
    ehd_connection_t *c;
    ehd_event_t *ev;
    c = r->connection;
    ev = c->event;
    pool = r->pool;

    if (error < EHD_HTTP_BAD_REQUEST) {
        /* 3XX */
        err = error - EHD_HTTP_MOVED_PERMANENTLY;

    } else if (error < EHD_HTTP_EHD_CODES) {
        /* 4XX */
        err = error - EHD_HTTP_BAD_REQUEST + 3;

    } else {
        /* 49X, 5XX */
        err = error - EHD_HTTP_EHD_CODES + 3 + 17;

        switch (error) {
            case EHD_HTTP_TO_HTTPS:
                r->headers_out.status = EHD_HTTP_BAD_REQUEST;
                error = EHD_HTTP_BAD_REQUEST;
                break;

            case EHD_HTTP_INVALID_HOST:
                r->headers_out.status = EHD_HTTP_NOT_FOUND;
                error = EHD_HTTP_NOT_FOUND;
                break;
        }
    }

    r->headers_out.content_type = ehd_list_push(&r->headers_out.headers);
    r->headers_out.content_type->key.len = sizeof("Content-Type") - 1;
    r->headers_out.content_type->key.data = (u_char *) "Content-Type";
    r->headers_out.content_type->value.len = sizeof("text/html") - 1;
    r->headers_out.content_type->value.data = (u_char *) "text/html";
    b = ehd_create_temp_buf(pool, error_pages[err].len + error_tail_t.len);
    memcpy(b->last, error_pages[err].data, error_pages[err].len);
    b->last += error_pages[err].len;
    memcpy(b->last , error_tail_t.data, error_tail_t.len);
    b->last += error_tail_t.len;
    r->headers_out.content_length_n = b->last - b->pos;
    r->headers_out.status = error;

    ehd_http_response_gen_header(r);
    r->response.last->next = ehd_pcalloc(pool,sizeof(ehd_chain_t));
    r->response.last = r->response.last->next;
    r->response.last->buf = b;
    c->write_handler = ehd_http_writer;
    if (ehd_add_event(ev, EHD_WRITE_EVENT, EPOLLET) == EHD_ERROR) {
        ehd_http_close_request(r);
        ehd_http_close_connection(c);
    }
    ehd_http_writer(c);
}

int ehd_http_response_gen_header(ehd_http_request_t *r) {
    ehd_http_response_t * resp;
    ehd_pool_t * pool;
    ehd_http_headers_out_t *h;
    ehd_buf_t * buf;
    int n,i;
    ehd_list_node_t *node;
    ehd_table_data_t *list, *t;
    u_char keyn,valuen;


    pool = r->pool;
    resp = &r->response;
    h = &r->headers_out;

    resp->data = ehd_pcalloc(pool,sizeof(ehd_chain_t));
    resp->last = r->response.data;
    buf = resp->data->buf = ehd_create_temp_buf(pool, 8192);
    resp->data->next = NULL;
    if (h->status_line.data != NULL) {
        n = snprintf((char *) buf->last, buf->end - buf->last, "HTTP/1.1 %d %s\r\n", h->status, h->status_line.data);
    } else {
        n = snprintf((char *) buf->last, buf->end - buf->last, "HTTP/1.1 %d\r\n", h->status);
    }
    buf->last += n;
    if (h->server == NULL) {
        h->content_type = t = ehd_list_push(&h->headers);
        t->key.data = (u_char *) "Server";
        t->key.len = sizeof("Server") - 1;
        t->value.data = (u_char *) EHD_VERSION;
        t->value.len = sizeof(EHD_VERSION) - 1;
    }
    if (h->content_length == NULL && h->content_length_n > 0) {
        h->content_type = t = ehd_list_push(&h->headers);
        t->key.data = (u_char *) "Content-Length";
        t->key.len = sizeof("Content-Length") - 1;
        t->value.data = ehd_pcalloc(pool, 20);
        t->value.len = (size_t) snprintf((char *) t->value.data, 20, "%ld", h->content_length_n);
    }
    for (node = &h->headers.head;node != NULL;node = node->next) {
        list = node->data;
        for (i = 0;i < node->ndata;i++) {
            t = &list[i];
            if (buf->end - buf->last > t->key.len + 2) {
                memcpy(buf->last, t->key.data, t->key.len);
                buf->last += t->key.len;
                *buf->last = (u_char) ':';
                buf->last++;
                *buf->last = (u_char) ' ';
                buf->last++;
                if (buf->end - buf->last > t->value.len + 2) {
                    memcpy(buf->last, t->value.data, t->value.len);
                    buf->last += t->value.len;
                    *buf->last = (u_char)CR;
                    buf->last++;
                    *buf->last = (u_char) LF;
                    buf->last++;
                    continue;
                }
            }
            /* palloc a bigger mem */
        }
    }
    if (buf->end - buf->last >= 2) {
        *buf->last = (u_char) CR;
        buf->last++;
        *buf->last = (u_char) LF;
        buf->last++;
        return EHD_OK;
    }
}


static void ehd_http_writer(ehd_connection_t *c) {
    ehd_http_request_t * r;
    r = c->data;
    ehd_chain_t *chain, *last;
    ehd_http_connection_t *hc;
    ssize_t n;
    int err;
    if (!r->response.begin) {
        r->response.last = r->response.data;
        r->response.begin = 1;
    }
    last = r->response.last;


    for(;;) {
        if (last->buf->type == 0) {
            n = ehd_send(c, last->buf->pos, last->buf->last - last->buf->pos);
            if (n > 0) {
                last->buf->pos += n;
                if (last->buf->pos == last->buf->last) {
                    if (last->next == NULL) {
                        ehd_http_keep_alive(r);
                        return;
                    } else {
                        last = last->next;
                    }
                } else {
                    continue;
                }
            } else if (n == 0) {
                ehd_http_close_request(r);
                ehd_http_close_connection(c);
                return;
            } else {
                err = errno;
                if (err == EAGAIN) {
                    return;
                }
                ehd_http_close_request(r);
                ehd_http_close_connection(c);
                return;
            }
        } else {
            n = sendfile(c->fd,
                     last->buf->file->fd,
                         &last->buf->file->sys_offset,
                         (size_t) (last->buf->file->size - last->buf->file->sys_offset));
//            last->buf->file->sys_offset = lseek(last->buf->file->fd, 0, SEEK_CUR );
            printf("%ld\n", last->buf->file->sys_offset);
            if (n > 0) {
                if (last->buf->file->sys_offset == last->buf->file->size) {
                    if (last->next == NULL) {
                        ehd_http_keep_alive(r);
                        return;
                    } else {
                        last = last->next;
                    }
                } else {
//                    last->buf->pos += n;
                    continue;
                }
            } else if (n == 0) {
                ehd_http_close_request(r);
                ehd_http_close_connection(c);
                return;
            } else {
                err = errno;
                if (err == EAGAIN) {
                    return;
                }
                ehd_http_close_request(r);
                ehd_http_close_connection(c);
                return;
            }
        }
    }
}
