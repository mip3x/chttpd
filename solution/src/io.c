#include <stdlib.h>
#include <stdio.h>

#include "../include/common.h"

static size_t get_opened_file_size(FILE* file) {
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    return (size_t)file_size;
}

char* read_file(const char* restrict file_path, size_t* file_size) {
    FILE* file = fopen(file_path, "r");

    if (file == NULL) {
        err("open file problem");
        return NULL;
    }

    *file_size = get_opened_file_size(file);
    char* file_content = malloc(*file_size + 1);

    if (file_content == NULL) {
        err("malloc file_content problem");
        fclose(file);
        return NULL;
    }

    fread(file_content, 1, *file_size, file);
    file_content[*file_size] = '\0';

    fclose(file);
    return file_content;
}
