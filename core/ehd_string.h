//
// Created by renmingxu on 12/21/17.
//

#ifndef UNTITLED_EHD_STRING_H
#define UNTITLED_EHD_STRING_H
#include <ehd_config.h>
#include <ehd_core.h>

typedef struct ehd_str_s ehd_str_t;

struct ehd_str_s {
    size_t len;
    u_char* data;
};

#define ehd_string(str) { sizeof(str) - 1, (u_char*) str}
#define ehd_null_string {0, NULL}


uint ehd_atoi(u_char *line, size_t n);

u_char *ehd_cpystrn(u_char *dst, u_char *src, size_t n);



#endif //UNTITLED_EHD_STRING_H
