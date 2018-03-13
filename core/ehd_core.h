//
// Created by renmingxu on 12/21/17.
//

#ifndef UNTITLED_EHD_CORE_H
#define UNTITLED_EHD_CORE_H
#define EHD_OK      0
#define EHD_ERROR   (-1)
#define EHD_AGAIN   (-2)

#define LF     (u_char) 10
#define CR     (u_char) 13
#define CRLF   "\x0d\x0a"

#define ehd_return_if_null(x,y,z) if (((x) = (y)) == NULL) return (z);

typedef struct ehd_pool_s ehd_pool_t;
typedef struct ehd_event_s ehd_event_t;
typedef struct ehd_cycle_s ehd_cycle_t;
typedef struct ehd_connection_s ehd_connection_t;
typedef struct ehd_listening_s ehd_listening_t;
typedef struct ehd_list_s ehd_list_t;
typedef struct ehd_conf_s ehd_conf_t;
typedef struct ehd_file_s ehd_file_t;

typedef void(*ehd_event_handler_fp)(ehd_event_t*ev);



#include <ehd_palloc.h>
#include <ehd_buf.h>
#include <ehd_string.h>
#include <ehd_file.h>
#include <ehd_list.h>
#include <ehd_array.h>
#include <ehd_conf.h>
#include <ehd_cycle.h>
#include <ehd_socket.h>
#include <ehd_event.h>
#include <ehd_connection.h>
#include <ehttpd.h>


#endif //UNTITLED_EHD_CORE_H
