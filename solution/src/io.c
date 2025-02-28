#include <stdlib.h>
#include <stdio.h>

#include "../include/status.h"

static size_t get_opened_file_size(FILE* file) {
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    return (size_t)file_size;
}

status read_file(char** const file_content, const char* restrict file_path, size_t* file_size) {
    FILE* file = fopen(file_path, "r");
    if (file == NULL) return OPEN_FILE_ERROR;

    *file_size = get_opened_file_size(file);
    *file_content = malloc(*file_size + 1);

    if (*file_content == NULL) {
        fclose(file);
        return MALLOC_FILE_CONTENT_ERROR;
    }

    fread(*file_content, 1, *file_size, file);
    (*file_content)[*file_size] = '\0';

    fclose(file);
    return OK;
}
