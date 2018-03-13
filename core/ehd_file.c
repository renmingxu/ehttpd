//
// Created by renmingxu on 12/27/17.
//
#include <ehd_config.h>
#include <ehd_core.h>
#include <sys/stat.h>


int ehd_file_open_r(ehd_file_t *file, ehd_str_t name) {
    int fd;

    if ((fd = open((char*)name.data, O_RDONLY)) == -1) {
        return EHD_ERROR;
    }

    file->fd = fd;
    file->name = name;
    file->offset = 0;
    file->sys_offset = 0;
    file->size = 0;
}
int ehd_file_open_rs(ehd_file_t *file, ehd_str_t name) {
    int fd;
    struct stat s;

    if ((fd = open((char*)name.data, O_RDONLY)) == -1) {
        return EHD_ERROR;
    }
    if (stat((char*)name.data, &s) == -1) {
        return EHD_ERROR;
    }
    file->size = s.st_size;
    file->fd = fd;
    file->name = name;
    file->offset = 0;
    file->sys_offset = 0;
}



