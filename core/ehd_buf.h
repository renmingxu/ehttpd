//
// Created by renmingxu on 12/21/17.
//

#ifndef UNTITLED_EHD_BUF_H
#define UNTITLED_EHD_BUF_H

#include <ehd_config.h>
#include <ehd_core.h>

#define EHD_DEFAULT_BUF_SIZE 4096

typedef struct ehd_buf_s ehd_buf_t;
typedef struct ehd_chain_s ehd_chain_t;

struct ehd_chain_s {
    ehd_buf_t * buf;
    ehd_chain_t * next;
};

struct ehd_buf_s {
    /* read position */
    u_char* pos;
    u_char* last;

    /* buffer size */
    u_char* start;
    u_char* end;

    int type;
    off_t file_pos;
    off_t file_last;
    ehd_file_t * file;

    /* content of the buf could be changed */
    unsigned temporary : 1;
};

ehd_buf_t* ehd_create_temp_buf(ehd_pool_t* pool, size_t size);

#endif //UNTITLED_EHD_BUF_H
