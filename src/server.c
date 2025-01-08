#include <netinet/in.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../include/server.h"
#include "../include/common.h"

static void handle_client(int client_fd, const char *web_root) {
    char buffer[4096];
    read(client_fd, buffer, sizeof(buffer));
    debug("Request:\n%s\n", buffer);

    const char *response = "HTTP/1.1 200 OK\r\n"
                           "Content-Type: text/plain\r\n"
                           "Content-Length: 13\r\n"
                           "\r\n"
                           "Hello, client";
    write(client_fd, response, strlen(response));
}

void destroy(server *srv) {
    close(srv->listen_fd);
    free(srv);
}

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

status start(server* srv) {
    if ((bind(srv->listen_fd, (struct sockaddr*)&srv->addr, sizeof(srv->addr))) < 0) {
        err("bind problem");
        close(srv->listen_fd);
        return ERROR;
    }

    if (listen(srv->listen_fd, 10) < 0) {
        err("listen problem");
        close(srv->listen_fd);
        return ERROR;
    }

    debug("Server is listening on port %d", ntohs(srv->addr.sin_port));

    while (1) {
        int client_fd = accept(srv->listen_fd, NULL, NULL);
        if (client_fd < 0) {
            err("accept problem");
            return ERROR;
        }

        handle_client(client_fd, srv->web_root);
        close(client_fd);
    }

    return OK;
}
