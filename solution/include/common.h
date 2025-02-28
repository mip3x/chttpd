#ifndef COMMON_H
#define COMMON_H

#include <stdbool.h>
#include <stddef.h>

#include "status.h"

void err(const char* fmt, ...);
void debug(const char* caller_func_name, const char* fmt, ...);

void print_err_message_by_status(status status);

char* bin2hex(const unsigned char* input, size_t len);

#endif
