#include <stdbool.h>
#include <stddef.h>
#include <string.h>

bool starts_with(const char* haystack, const char* needle) {
    size_t len_needle = strlen(needle), lenstr = strlen(haystack);
    return lenstr < len_needle ? false : memcmp(needle, haystack, len_needle) == 0;
}

size_t get_occurrences_number(const char* haystack, const char* needle) {
    size_t count = 0;
    const char* tmp = haystack;

    tmp = strstr(tmp, needle);
    while (tmp) {
        count++;
        tmp++;
        tmp = strstr(tmp, needle);
    }
    return count;
}
