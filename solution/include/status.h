#ifndef STATUS_H
#define STATUS_H

typedef enum {
    OK,
    OPEN_FILE_ERROR,
    MALLOC_FILE_CONTENT_ERROR,
    READ_FILE_ERROR,
    SERVER_POINTER_ERROR,
    SOCKET_ERROR,
    SETSOCKOPT_ERROR,
    BIND_ERROR,
    LISTEN_ERROR,
    ACCEPT_ERROR
} status;

#endif
