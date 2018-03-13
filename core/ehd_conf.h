//
// Created by renmingxu on 12/27/17.
//

#ifndef EHTTPD_EHD_CONF_H
#define EHTTPD_EHD_CONF_H

#include <ehd_config.h>
#include <ehd_core.h>



struct ehd_conf_s {
    uint connection_num;
    ehd_list_t addrs_str;
    ehd_list_t ports_int;
    ehd_str_t root;
    ehd_str_t index;
    ehd_pool_t * pool;
};


int ehd_conf_init(ehd_conf_t * conf, ehd_file_t *file);

#endif //EHTTPD_EHD_CONF_H
