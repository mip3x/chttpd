#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../include/route.h"
#include "../include/networking/server.h"
#include "../include/networking/transport/http_request.h"
#include "../include/networking/transport/http_response.h"
#include "../include/common.h"
#include "../include/dictionary.h"
#include "../include/io.h"
#include "../include/404.h"

static char* handle_route(const route incoming_route) {
    char* full_path = NULL;
    char* file_content_type = NULL;
    char* body = malloc(sizeof(char*));
    size_t content_length = 0;
    char* response = NULL;

    if (incoming_route.file_path == NULL) {
        debug(__func__, "page not found");

        asprintf(&full_path, "%s%s", DEFAULT_WEB_ROOT_PATH, DEFAULT_404_HTML_FILE_PATH);
        debug(__func__, "file path: %s", full_path);
        status read = read_file(&body, full_path, &content_length);

        if (read != OK) {
            print_err_message_by_status(read);
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

        asprintf(&full_path, "%s%s", incoming_route.web_root, incoming_route.file_path);
        debug(__func__, "file path: %s", full_path);
        status read = read_file(&body, full_path, &content_length);

        if (read != OK) {
            print_err_message_by_status(read);
            return handle_route((route){.file_path = NULL});
        }

        if (strstr(incoming_route.file_path, "html")) file_content_type = CONTENT_TYPE_HTML;
        if (strstr(incoming_route.file_path, "css")) file_content_type = CONTENT_TYPE_CSS;
        if (strstr(incoming_route.file_path, "js")) file_content_type = CONTENT_TYPE_JS;

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

static void handle_client(int client_fd) {
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
    if (!srv) return SERVER_POINTER_ERROR;

    srv->port = port != 0 ? port : DEFAULT_SERVER_PORT;
    srv->web_root = web_root ? web_root : DEFAULT_WEB_ROOT_PATH;

    bzero(&srv->addr, sizeof(srv->addr));
    srv->addr.sin_family = AF_INET;
    srv->addr.sin_port = htons(srv->port);
    srv->addr.sin_addr.s_addr = htonl(INADDR_ANY);

    srv->listen_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (srv->listen_fd == -1) {
        free(srv);
        return SOCKET_ERROR;
    }
    
    int yes = 1;
    if (setsockopt(srv->listen_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
        close(srv->listen_fd);
        return SETSOCKOPT_ERROR;
    }

    if ((bind(srv->listen_fd, (struct sockaddr*)&srv->addr, sizeof(srv->addr))) == -1) {
        close(srv->listen_fd);
        return BIND_ERROR;
    }

    return OK;
}

status launch_server(server* srv) {
    if (listen(srv->listen_fd, 10) == -1) {
        close(srv->listen_fd);
        return LISTEN_ERROR;
    }

    debug(__func__, "Server is listening on port %d", ntohs(srv->addr.sin_port));

    for (;;) {
        int client_fd = accept(srv->listen_fd, NULL, NULL);
        if (client_fd == -1) {
            return ACCEPT_ERROR;
        }

        handle_client(client_fd);
        close(client_fd);
    }

    return OK;
}
