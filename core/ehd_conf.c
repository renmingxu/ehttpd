//
// Created by renmingxu on 12/27/17.
//

#include <ehd_config.h>
#include <ehd_core.h>

uint ehd_threaded;

int ehd_conf_init(ehd_conf_t *conf, ehd_file_t *file) {
    ehd_pool_t * pool;
    ehd_list_t * addrs;
    ehd_list_t * ports;
    ehd_str_t * addr;
    ehd_str_t addr1 = ehd_string("0.0.0.0");
    ehd_str_t addr2 = ehd_string("127.0.1.1");
    ehd_str_t index = ehd_string("index.html");
    uint * port;
    uint port1 = 8080;
    uint port2 = 9999;
    ehd_str_t root = ehd_string("/var/www/html/");
    if (conf->pool == NULL) {
        conf->pool = ehd_create_pool_default_size();
    }
    pool = conf->pool;
    conf->connection_num = 1024;
    addrs = &conf->addrs_str;
    ehd_list_init(addrs, pool, 4, sizeof(ehd_str_t));

    ehd_threaded = 0;

    addr = ehd_list_push(addrs);
    *addr = addr1;
    addr = ehd_list_push(addrs);
    *addr = addr2;

    ports = &conf->ports_int;
    ehd_list_init(ports, pool, 4, sizeof(int));

    port = ehd_list_push(ports);
    *port = port1;
    port = ehd_list_push(ports);
    *port = port2;
    printf("asdf\n");
    fflush(stdout);


    conf->root = root;
    conf->index = index;

}
