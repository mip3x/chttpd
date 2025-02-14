#ifndef ROUTE_H
#define ROUTE_H

#include "common.h"

typedef struct {
#define CONFIG_WEB_ROOT "__CONFIG_WEB_ROOT__"
    char* web_root;
    char* mapping;
    char* file_path;
    void (*handler)(int client_fd);
} route;

void print_route(route route);

#define PRINT_ROUTE(route) \
    debug(__func__, "route:\n\tweb_root: %s\n\tmapping: %s\n\tfile_path: %s", \
        (route).web_root, \
        (route).mapping, \
        (route).file_path \
    );

#endif
