#ifndef COMMON_H
#define COMMON_H

#include <stdbool.h>
#include <stddef.h>

void err(const char* fmt, ...);
void debug(const char* caller_func_name, const char* fmt, ...);
bool starts_with(const char *pre, const char *str);

char* bin2hex(const unsigned char* input, size_t len);

#endif
