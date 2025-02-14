#ifndef ROUTE_H
#define ROUTE_H

#include "common.h"
#include "networking/server.h"

extern char* CONFIG_WEB_ROOT_PATH;

typedef struct {
    char* web_root;
    char* mapping;
    char* file_path;
    void (*handler)(int client_fd);
} route;

#define PRINT_ROUTE(route) \
    debug(__func__, "route:\n\tweb_root: %s\n\tmapping: %s\n\tfile_path: %s", \
        (route).web_root, \
        (route).mapping, \
        (route).file_path \
    );

void print_route(route* route);
void set_config_web_root_if_needed(route* route);

#endif
