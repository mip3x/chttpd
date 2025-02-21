#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>
#include <stdint.h>

#include "../status.h"

typedef struct server {
    int listen_fd;
#define DEFAULT_SERVER_PORT 14880
    uint16_t port;
#define DEFAULT_WEB_ROOT_PATH "./www/"
    char* web_root;
    struct sockaddr_in addr;
} server;

status init_server(server* srv, uint16_t port, char* web_root);
status launch_server(server* srv);

void destroy_server(server* srv);

#endif
