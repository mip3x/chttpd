#ifndef ROUTE_H
#define ROUTE_H

typedef struct {
    char* mapping;
    char* file_path;
    void (*handler)(int client_fd);
} route;

#endif
