#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>
#include <stdint.h>

#include "../status.h"

#define DEFAULT_404_FILE_PATH "default/404.html"

typedef struct server {
    int listen_fd;
#define DEFAULT_SERVER_PORT 14880
    uint16_t port;
#define DEFAULT_WEB_ROOT_PATH "./www/"
    const char* web_root;
    struct sockaddr_in addr;
} server;

/*void (*register_routes)(char *(route_handler)(struct http_request* request), char* uri, int num_methods, ...);*/

/*char* example_handler(struct http_request* request) {*/
/*    return (char*)"meow";*/
/*}*/
/**/
/*int main() {*/
/*    server srv;*/
/*    srv.register_routes(example_handler, "/example", 2, 0, 1);*/
/*}*/

status init_server(server* srv, uint16_t port, const char* web_root);
status launch_server(server* srv);

void destroy_server(server* srv);

#endif
