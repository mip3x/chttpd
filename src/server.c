#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../include/networking/server.h"
#include "../include/networking/transport/http_request.h"
#include "../include/common.h"

static void handle_client(int client_fd, const char *web_root) {
    char buffer[4096];
    read(client_fd, buffer, sizeof(buffer));
    http_request request = parse_raw_request(buffer);
    /*debug("Request:\n%s\n", buffer);*/
    debug("Method: %d\nURI: %s\nVersion: %.1f\n", 
      request.method, 
      request.uri ? request.uri : "(null)", 
      request.version);

    const char *response = "HTTP/1.1 200 OK\r\n"
                           "Content-Type: text/plain\r\n"
                           "Content-Length: 13\r\n"
                           "\r\n"
                           "Hello, client";
    write(client_fd, response, strlen(response));
}

void destroy_server(server *srv) {
    close(srv->listen_fd);
    free(srv);
}

status init_server(server* srv, uint16_t port, const char* web_root) {
    if (!srv) {
        err("server pointer is NULL");
        return ERROR;
    }

    srv->port = port != 0 ? port : DEFAULT_SERVER_PORT;
    srv->web_root = web_root ? web_root : DEFAULT_WEB_ROOT_PATH;

    bzero(&srv->addr, sizeof(srv->addr));
    srv->addr.sin_family = AF_INET;
    srv->addr.sin_port = htons(srv->port);
    srv->addr.sin_addr.s_addr = htonl(INADDR_ANY);

    srv->listen_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (srv->listen_fd == -1) {
        err("socket problem");
        free(srv);
        return ERROR;
    }
    
    int yes = 1;
    if (setsockopt(srv->listen_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
        err("setsockopt problem");
        close(srv->listen_fd);
        return ERROR;
    }

    if ((bind(srv->listen_fd, (struct sockaddr*)&srv->addr, sizeof(srv->addr))) == -1) {
        err("binding problem");
        close(srv->listen_fd);
        return ERROR;
    }

    return OK;
}

status launch_server(server* srv) {
    if (listen(srv->listen_fd, 10) == -1) {
        err("listen problem");
        close(srv->listen_fd);
        return ERROR;
    }

    debug("Server is listening on port %d", ntohs(srv->addr.sin_port));

    for (;;) {
        int client_fd = accept(srv->listen_fd, NULL, NULL);
        if (client_fd == -1) {
            err("accept problem");
            return ERROR;
        }

        handle_client(client_fd, srv->web_root);
        close(client_fd);
    }

    return OK;
}
