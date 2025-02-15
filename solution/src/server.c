#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../include/networking/server.h"
#include "../include/networking/transport/http_request.h"
#include "../include/networking/transport/http_response.h"
#include "../include/common.h"
#include "../include/dictionary.h"
#include "../include/io.h"

static char* handle_route(const route route) {
    char* full_path = NULL;
    char* file_content_type = NULL;
    char* body = NULL;
    size_t content_length = 0;
    char* response = NULL;

    if (route.file_path == NULL) {
        debug(__func__, "page not found");

        asprintf(&full_path, "%s%s", DEFAULT_WEB_ROOT_PATH, DEFAULT_404_FILE_PATH);
        debug(__func__, "file path: %s", full_path);
        body = read_file(full_path, &content_length);

        if (body == NULL) {
            body = strdup("404 should be here");
            content_length = strlen(body);
        }

        asprintf(&response, "%s%sContent-Length: %zu\r\n\r\n%s",
                 HTTP_404,
                 CONTENT_TYPE_HTML,
                 content_length,
                 body
        );
    }
    else {
        debug(__func__, "page found");

        asprintf(&full_path, "%s%s", route.web_root, route.file_path);
        debug(__func__, "file path: %s", full_path);
        body = read_file(full_path, &content_length);

        if (body == NULL) {
            err("file read problem");
            return NULL;
        }

        if (strstr(route.file_path, "html")) file_content_type = CONTENT_TYPE_HTML;
        if (strstr(route.file_path, "css")) file_content_type = CONTENT_TYPE_CSS;
        if (strstr(route.file_path, "js")) file_content_type = CONTENT_TYPE_JS;

        asprintf(&response, "%s%sContent-Length: %zu\r\n\r\n%s",
                 HTTP_200,
                 file_content_type,
                 content_length,
                 body
        );
    }

    free(body);

    return response;
}

static void handle_client(int client_fd, char* web_root) {
    char buffer[4096];
    read(client_fd, buffer, sizeof(buffer));
    http_request request = parse_raw_request(buffer);

    debug(__func__, "Method: %d\nURI: %s\nVersion: %.1f", 
        request.method, 
        request.uri ? request.uri : "(null)", 
        request.version
    );

    route found_route = { NULL };
    if (request.uri != NULL) {
        struct nlist* lookup_result = lookup(request.uri);
        if (lookup_result != NULL) found_route = lookup_result->entry;
    }

    char* response = handle_route(found_route);
    if (response == NULL) return;

    puts(response);
    write(client_fd, response, strlen(response));

    free(response);
}

void destroy_server(server* srv) {
    close(srv->listen_fd);
    free(srv);
}

status init_server(server* srv, uint16_t port, char* web_root) {
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

    debug(__func__, "Server is listening on port %d", ntohs(srv->addr.sin_port));

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
