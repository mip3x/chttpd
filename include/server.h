#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>
#include <stdint.h>

#define SERVER_PORT 14880
#define WEB_ROOT_PATH "../www/"

typedef struct {
    int listen_fd;
    const char* web_root;
    struct sockaddr_in addr;
} server;

server* init(uint16_t port, const char* web_root);

typedef enum {
    OK,
    ERROR
} status;

status start(server* srv);
void destroy(server* srv);

#endif
