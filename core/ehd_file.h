//
// Created by renmingxu on 12/27/17.
//

#ifndef EHTTPD_EHD_FILE_H
#define EHTTPD_EHD_FILE_H

#include <ehd_config.h>
#include <ehd_core.h>

struct ehd_file_s {
    int fd;
    ehd_str_t name;

    off_t offset;

    off_t sys_offset;

    off_t size;
};


int ehd_file_open_r(ehd_file_t * file, ehd_str_t name);
int ehd_file_open_rs(ehd_file_t * file, ehd_str_t name);


#endif //EHTTPD_EHD_FILE_H
