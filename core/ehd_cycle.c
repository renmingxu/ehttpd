//
// Created by renmingxu on 12/25/17.
//
#include <ehd_config.h>
#include <ehd_core.h>
#include <ehd_http.h>

volatile ehd_cycle_t * ehd_cycle;

int ehd_cycle_init(ehd_cycle_t * cycle, ehd_conf_t * conf) {
    uint i;
    ehd_pool_t * pool;
    if (cycle->pool == NULL) {
        cycle->pool = ehd_create_pool_default_size();
    }
    pool = cycle->pool;

    cycle->conf = conf;
    ehd_array_init(&cycle->listening, pool, EHD_CYCLE_LISTENING_SIZE, sizeof(ehd_listening_t));
    if (ehd_listening_init(cycle, conf) == EHD_ERROR) {
        perror("ehd_listening_init");
        return EHD_ERROR;
    }
    if (ehd_open_listening_socket(cycle) == EHD_ERROR) {
        perror("ehd_open_listening_socket");
        return EHD_ERROR;
    }
    if (ehd_event_init(cycle) == EHD_ERROR) {
        perror("ehd_event_init");
        return EHD_ERROR;
    }

    if (ehd_http_init(cycle) == EHD_ERROR) {
        return EHD_ERROR;
    }

    return EHD_OK;
}


