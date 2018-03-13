cmake_minimum_required(VERSION 3.8)
project(ehttpd)

set(CMAKE_C_STANDARD 11)

INCLUDE_DIRECTORIES(core/ event/ http/ test/)
set(SOURCE_FILES
        core/ehd_config.h
        core/ehd_core.h
        core/ehd_buf.h
        core/ehd_list.h
        core/ehd_palloc.h
        core/ehd_string.h
        core/ehttpd.h
        core/ehd_buf.c
        core/ehd_list.c
        core/ehd_palloc.c
        core/ehd_string.c
        core/ehttpd.c
        event/ehd_event.c
        event/ehd_event.h
        http/ehd_http_request.h
        http/ehd_http_request.c
        core/ehd_connection.h
        core/ehd_cycle.c
        core/ehd_cycle.h
        core/ehd_array.c
        core/ehd_array.h
        test/test_all.c
        test/test_all.h core/ehd_connection.c core/ehd_socket.h core/ehd_socket.c core/ehd_conf.c core/ehd_conf.h core/ehd_file.h core/ehd_file.c event/ehd_event_echo.c event/ehd_epoll.h event/ehd_epoll.c event/ehd_event_accept.c http/ehd_http.c http/ehd_http.h http/ehd_http_parse.c http/ehd_http_core.c http/ehd_http_core.h)
add_executable(ehttpd ${SOURCE_FILES})

