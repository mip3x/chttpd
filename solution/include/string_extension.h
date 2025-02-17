#ifndef STRING_EXTENSION_H
#define STRING_EXTENSION_H

#include <stdbool.h>
#include <stddef.h>

bool starts_with(const char* prefix, const char* str);
size_t get_occurrences_number(const char* haystack, const char* needle);

#endif
