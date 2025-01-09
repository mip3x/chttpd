#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "../include/common.h"
#include "../include/networking/transport/http_request.h"

static enum http_methods parse_http_method(const char* method) {
    if (strcmp(method, "GET") == 0) return GET;
    if (strcmp(method, "POST") == 0) return POST;
    if (strcmp(method, "PUT") == 0) return PUT;
    if (strcmp(method, "HEAD") == 0) return HEAD;
    if (strcmp(method, "PATCH") == 0) return PATCH;
    if (strcmp(method, "DELETE") == 0) return DELETE;
    if (strcmp(method, "CONNECT") == 0) return CONNECT;
    if (strcmp(method, "OPTIONS") == 0) return OPTIONS;
    if (strcmp(method, "TRACE") == 0) return TRACE;

    return UINT8_MAX;
}

http_request parse_raw_request(char* raw_request) {
    http_request request = {0};

    char* request_line = strtok(raw_request, "\r\n");
    if (request_line == NULL) {
        err("invalid HTTP request format\n");
        return request;
    }

    char* method = strtok(request_line, " ");
    char* uri = strtok(NULL, " ");
    char* version_str = strtok(NULL, " ");

    if (!method || !uri || !version_str) {
        err("failed to parse request line\n");
        return request;
    }

    request.method = parse_http_method(method);
    if (request.method == UINT8_MAX) {
        err("unknown HTTP method: %s\n", method);
        return request;
    }

    request.uri = strdup(uri);

    if (strncmp(version_str, "HTTP/", 5) != 0 || !isdigit(version_str[5])) {
        err("invalid HTTP version: %s\n", version_str);
        return request;
    }

    request.version = strtof(version_str + 5, NULL);

    return request;
}
