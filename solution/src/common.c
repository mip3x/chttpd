#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/status.h"

void err(const char* fmt, ...) {
    int     errno_save;
    va_list ap;

    errno_save = errno;
    va_start(ap, fmt);
    fprintf(stderr, "[ERROR] ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    fflush(stderr);

    if (errno_save != 0) {
        fprintf(stderr, "(errno = %d) : %s\n", errno_save, strerror(errno_save));
        fprintf(stderr, "\n");
        fflush(stderr);
    }

    va_end(ap);
}

void print_err_message_by_status(status status) {
    switch (status) {
        case OK:
        case OPEN_FILE_ERROR: {
            err("Error occurred while opening file");
            break;
        }
        case MALLOC_FILE_CONTENT_ERROR: {
            err("Error occurred while allocating space for file content");
            break;
        }
        case READ_FILE_ERROR: {
            err("Error occurred while reading file");
            break;
        }
        case SERVER_POINTER_ERROR: {
            err("Error occurred while creating server");
            break;
        }
        case SOCKET_ERROR: {
            err("Error occurred while opening socket");
            break;
        }
        case SETSOCKOPT_ERROR: {
            err("Error occurred while setting up socket");
            break;
        }
        case BIND_ERROR: {
            err("Error occurred while binding socket");
            break;
        }
        case LISTEN_ERROR: {
            err("Error occurred while listening");
            break;
        }
        case ACCEPT_ERROR: {
            err("Error occurred while accepting new connection");
            break;
        }
    }
}

void debug(const char* caller_func_name, const char* fmt, ...) {
    va_list ap;

    va_start(ap, fmt);
    fprintf(stdout, "[INFO (%s)] ", caller_func_name);
    vfprintf(stdout, fmt, ap);
    fprintf(stdout, "\n");
    fflush(stdout);

    va_end(ap);
}

char* bin2hex(const unsigned char* input, size_t len) {
    char* result;
    char* hexits = "0123456789ABCDEF";

    if (input == NULL || len <= 0) return NULL;

    int result_length = (len * 3) + 1;

    result = malloc(result_length);
    if (result == NULL) return NULL;

    bzero(result, result_length);

    for (size_t i = 0; i < len; i++) {
        result[i * 3]     = hexits[input[i] >> 4];
        result[i * 3 + 1] = hexits[input[i] & 0x0F];
        result[i * 3 + 2] = ' ';
    }

    return result;
}
