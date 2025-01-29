#include <stdio.h>
#include <string.h>

#include "../include/config_parser.h"
#include "../include/io.h"

server parse_config(const char *file_path) {
    size_t file_size = 0;
    char* file_content = read_file(file_path, &file_size);

    server srv = {0};
    char* line = strtok(file_content, "\n");
    while (line) {
        if (line[0] == '#') {
            line = strtok(NULL, "\n");
            continue;
        }

        char key[128], value1[256], value2[256];
        int tokens = sscanf(line, "%s %s %s", key, value1, value2);

        if (tokens >= 2) {
            if (strcmp(key, "port") == 0) srv.port = atoi(value1);
            else if (strcmp(key, "web_root") == 0) srv.web_root = value1;
        }
    }

    return srv;
}
