#ifndef COMMON_H
#define COMMON_H

#include <stddef.h>

_Noreturn void err(const char* fmt, ...);
char* bin2hex(const unsigned char* input, size_t len);

#endif
