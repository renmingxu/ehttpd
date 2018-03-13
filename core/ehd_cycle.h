//
// Created by renmingxu on 12/25/17.
//

#ifndef EHTTPD_EHD_CYCLE_H
#define EHTTPD_EHD_CYCLE_H
#include <ehd_config.h>
#include <ehd_core.h>

#define EHD_CYCLE_LISTENING_SIZE 4
extern volatile ehd_cycle_t *ehd_cycle;
extern uint ehd_threaded; // init in ehd_conf_init()

struct ehd_cycle_s {
    ehd_pool_t *pool;

    ehd_array_t listening;

    uint connection_num;
    ehd_connection_t * connections;
    ehd_event_t * events;

    ehd_str_t root;
    ehd_conf_t * conf;

};



int ehd_cycle_init(ehd_cycle_t * cycle, ehd_conf_t * conf);
#endif //EHTTPD_EHD_CYCLE_H
