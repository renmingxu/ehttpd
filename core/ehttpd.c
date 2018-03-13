//
// Created by renmingxu on 12/21/17.
//

#include <ehd_config.h>
#include <ehd_core.h>
#include <test_all.h>


int main(int argc, char ** argv) {
    /*
    int t;
    if (( t = test_all())<0) {
        printf("test fail %d\n", t);
    } else {
        printf("test success %d\n", t);
    }
     */
    ehd_pool_t * pool = ehd_create_pool_default_size();
    ehd_conf_t * conf = ehd_palloc(pool, sizeof(ehd_conf_t));
    ehd_cycle_t * cycle = ehd_palloc(pool, sizeof(ehd_cycle_t));

    conf->pool = pool;
    cycle->pool = pool;


    ehd_file_t conf_file;
    ehd_str_t conf_file_name = ehd_string("ehttpd.conf");
    //ehd_file_open_r(&conf_file, conf_file_name);
    ehd_conf_init(conf, &conf_file);
    ehd_cycle_init(cycle, conf);
    ehd_cycle = cycle;

    ehd_process_event(cycle);
}

