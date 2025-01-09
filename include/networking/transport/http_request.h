#ifndef REQUEST_H
#define REQUEST_H

#include <stdint.h>

enum http_methods {
    GET,
    POST,
    PUT,
    HEAD,
    PATCH,
    DELETE,
    CONNECT,
    OPTIONS,
    TRACE
};

typedef struct {
    uint8_t method;
    char* uri;
    float version;
} http_request;

http_request parse_raw_request(char* raw_request);

#endif
