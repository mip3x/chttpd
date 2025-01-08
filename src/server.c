#include <netinet/in.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "../include/server.h"
#include "../include/common.h"

server* init(uint16_t port, const char* web_root) {
    server* srv = malloc(sizeof(server));

    if (!srv) {
        err("server allocating problem");
        return NULL;
    }

    srv->web_root = web_root;
    srv->listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (srv->listen_fd < 0) {
        err("socket problem");
        free(srv);
        return NULL;
    }

    bzero(&srv->addr, sizeof(srv->addr));
    srv->addr.sin_family = AF_INET;
    srv->addr.sin_port = htons(port);
    srv->addr.sin_addr.s_addr = htonl(INADDR_ANY);

    return srv;
}
