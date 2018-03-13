//
// Created by renmingxu on 12/21/17.
//

#ifndef EHTTPD_EHD_HTTP_REQUEST_H
#define EHTTPD_EHD_HTTP_REQUEST_H

#include <ehd_config.h>
#include <ehd_palloc.h>
#include <ehd_connection.h>



#define EHD_HTTP_PARSE_INVALID_METHOD       10
#define EHD_HTTP_PARSE_INVALID_REQUEST      11
#define EHD_HTTP_PARSE_TOO_LONG_URI         12
#define EHD_HTTP_PARSE_INVALID_09_METHOD    13


#define EHD_HTTP_PARSE_INVALID_HEADER       15
#define EHD_HTTP_PARSE_HEADER_DONE          16


#define EHD_HTTP_GET                        1
#define EHD_HTTP_HEAD                       2
#define EHD_HTTP_POST                       3


#define EHD_HTTP_OK                        200
#define EHD_HTTP_PARTIAL_CONTENT           206

#define EHD_HTTP_SPECIAL_RESPONSE          300
#define EHD_HTTP_MOVED_PERMANENTLY         301
#define EHD_HTTP_MOVED_TEMPORARILY         302
#define EHD_HTTP_NOT_MODIFIED              304

#define EHD_HTTP_BAD_REQUEST               400
#define EHD_HTTP_FORBIDDEN                 403
#define EHD_HTTP_NOT_FOUND                 404
#define EHD_HTTP_NOT_ALLOWED               405
#define EHD_HTTP_REQUEST_TIME_OUT          408
#define EHD_HTTP_REQUEST_ENTITY_TOO_LARGE  413
#define EHD_HTTP_REQUEST_URI_TOO_LARGE     414
#define EHD_HTTP_RANGE_NOT_SATISFIABLE     416


#define EHD_HTTP_EHD_CODES                 EHD_HTTP_TO_HTTPS
#define EHD_HTTP_TO_HTTPS                  497
#define EHD_HTTP_INVALID_HOST              498
#define EHD_HTTP_CLIENT_CLOSED_REQUEST     499
#define EHD_HTTP_INTERNAL_SERVER_ERROR     500
#define EHD_HTTP_NOT_IMPLEMENTED           501
#define EHD_HTTP_BAD_GATEWAY               502
#define EHD_HTTP_SERVICE_UNAVAILABLE       503
#define EHD_HTTP_GATEWAY_TIME_OUT          504

typedef enum {
    EHD_HTTP_INITING_REQUEST_STATE = 0,
    EHD_HTTP_READING_REQUEST_STATE,
    EHD_HTTP_PROCESS_REQUEST_STATE,

    EHD_KEEP_ALIVE_STATE,
}ehd_http_state_e;

typedef struct ehd_http_request_s ehd_http_request_t;

typedef struct {
    ehd_str_t   name;
    size_t        offset;
} ehd_http_header_t;

typedef struct {
    ehd_http_request_t * request;

}ehd_http_connection_t;

typedef struct {
    ehd_list_t headers;
    ehd_table_data_t * host;
    ehd_table_data_t * connection;
    ehd_table_data_t * if_modified_since;
    ehd_table_data_t * content_length;
    ehd_table_data_t * user_agent;
    ehd_table_data_t * referer;
    ehd_table_data_t * range;
    ehd_table_data_t * accept_encoding;
    ehd_table_data_t * keep_alive;
    ehd_table_data_t * authorization;
    ehd_array_t cookies;
    ssize_t  content_length_n;
}ehd_http_headers_in_t;

typedef struct {
    ehd_list_t headers;
    uint status;
    ehd_str_t status_line;
    ehd_table_data_t *server;
    ehd_table_data_t *date;
    ehd_table_data_t *content_type;
    ehd_table_data_t *content_length;
    ehd_table_data_t *content_ranges;
    ehd_table_data_t *etag;
    ehd_table_data_t *charset;

    time_t date_time;
    time_t last_moddified_time;

    off_t content_length_n;
}ehd_http_headers_out_t;

typedef struct ehd_http_response_s ehd_http_response_t;
struct ehd_http_response_s {
    ehd_chain_t *data;
    ehd_chain_t *last;
    unsigned begin:1;
};

struct ehd_http_request_s {
    ehd_pool_t * pool;

    ehd_http_connection_t * http_connection;
    ehd_http_state_e http_state;
    ehd_conf_t *conf;

    ehd_connection_t * connection;

    ehd_buf_t * header_in_buf;
    ehd_http_headers_in_t headers_in;
    int headers_n;
    ehd_http_headers_out_t headers_out;
    uint method;
    uint http_version;
    uint http_major;
    uint http_minor;

    ehd_str_t request_line;
    ehd_str_t uri;
    ehd_str_t unparsed_uri;
    ehd_str_t args;
    ehd_str_t exten;
    ehd_str_t method_name;

    ehd_http_response_t response;

    uint port;
    ehd_str_t * port_text;
    ehd_str_t * server_name;
    off_t sent;


    /* used to parse http headers */
    uint state;
    u_char *request_start;
    u_char *method_end;
    u_char *uri_start;
    u_char *schema_start;
    u_char *schema_end;
    u_char *host_start;
    u_char *host_end;
    u_char *port_end;
    u_char *uri_end;
    u_char *args_start;
    u_char *uri_ext;
    u_char *request_end;
    u_char *header_end;


    u_char *header_name_start;
    u_char *header_name_end;
    unsigned proxy:1;
    unsigned complex_uri:1;
    u_char *header_start;
};



#endif //EHTTPD_EHD_HTTP_REQUEST_H
