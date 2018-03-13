//
// Created by renmingxu on 12/21/17.
//

#include <ehd_config.h>
#include <ehd_core.h>


uint ehd_atoi(u_char *line, size_t n) {
    uint value;

    if (n == 0) {
        return EHD_ERROR;
    }

    for (value = 0; n--;line++) {
        if (*line < '0' || *line > '9') {
            return EHD_ERROR;
        }

        value = value * 10 + (*line - '0');
    }

    if (value < 0) {
        return EHD_ERROR;
    }

    return value;
}


u_char *ehd_cpystrn(u_char *dst, u_char *src, size_t n)
{
    if (n == 0) {
        return dst;
    }

    for (/* void */; --n; dst++, src++) {
        *dst = *src;

        if (*dst == '\0') {
            return dst;
        }
    }

    *dst = '\0';

    return dst;
}


