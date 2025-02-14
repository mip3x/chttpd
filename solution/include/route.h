#ifndef ROUTE_H
#define ROUTE_H

typedef struct {
#define CONFIG_WEB_ROOT "__CONFIG_WEB_ROOT__"
    char* web_root;
    char* mapping;
    char* file_path;
    void (*handler)(int client_fd);
} route;

void print_route(route route);

#endif
