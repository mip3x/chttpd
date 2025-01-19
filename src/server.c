#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../include/networking/server.h"
#include "../include/networking/transport/http_request.h"
#include "../include/common.h"
#include "../include/dictionary.h"
#include "../include/io.h"

#define HTTP_404 "HTTP/1.1 404 Not Found\r\n"
#define HTTP_200 "HTTP/1.1 200 OK\r\n"

#define CONTENT_TYPE_TEXT "Content-Type: text/plain\r\n"
#define CONTENT_TYPE_HTML "Content-Type: text/html; charset=UTF-8\r\n"
#define CONTENT_TYPE_CSS "Content-Type: text/css; charset=UTF-8\r\n"
#define CONTENT_TYPE_JS "Content-Type: text/js; charset=UTF-8\r\n"

static char* handle_file_path(const char* file_path, const char* web_root) {
    char* full_path = NULL;
    char* file_content_type = NULL;
    char* body = NULL;
    size_t content_length = 0;
    char* response = NULL;

    if (file_path == NULL) {
        debug(__func__, "page not found\n");

        asprintf(&full_path, "%s%s", web_root, DEFAULT_404_FILE_PATH);
        debug("file path: %s\n", full_path);
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
        debug(__func__, "page found\n");

        asprintf(&full_path, "%s%s", web_root, file_path);
        debug(__func__, "file path: %s\n", full_path);
        body = read_file(full_path, &content_length);

        if (body == NULL) {
            err("file read problem");
            return NULL;
        }

        if (strstr(file_path, "html")) file_content_type = CONTENT_TYPE_HTML;
        if (strstr(file_path, "css")) file_content_type = CONTENT_TYPE_CSS;

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

static void handle_client(int client_fd, const char *web_root) {
    char buffer[4096];
    read(client_fd, buffer, sizeof(buffer));
    http_request request = parse_raw_request(buffer);
    /*debug("Request:\n%s\n", buffer);*/
    debug(__func__, "Method: %d\nURI: %s\nVersion: %.1f\n", 
      request.method, 
      request.uri ? request.uri : "(null)", 
      request.version
    );

    char* file_path = NULL;
    if (request.uri != NULL) {
        struct nlist* lookup_result = lookup(request.uri);
        if (lookup_result != NULL) file_path = lookup_result->entry.file_path;
    }

    char* response = handle_file_path(file_path, web_root);
    puts(response);
    write(client_fd, response, strlen(response));

    free(response);
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
