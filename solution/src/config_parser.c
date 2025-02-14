#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "../include/config_parser.h"
#include "../include/route.h"
#include "../include/dictionary.h"
#include "../include/io.h"
#include "../include/common.h"

server parse_config(const char *file_path) {
    size_t file_size = 0;
    char* file_content = read_file(file_path, &file_size);

    bool http_block = 0;
    bool route_block = 0;

    server srv = {0};
    route current_route = {.mapping = NULL, .file_path = NULL, .web_root = NULL};

    char* token = strtok(file_content, " \n\t");

    while (token) {
        /*debug(__func__, "token: %s", token);*/
        if (strcmp(token, "{") == 0) {}
        else if (strcmp(token, "http") == 0) http_block = true;
        else if (strcmp(token, "route") == 0) {
            memset(&current_route, 0, sizeof(current_route));
            route_block = true;
        }
        else if (strcmp(token, "}") == 0) {
            if (route_block) {
                install(current_route);
                route_block = false;
            }
            else http_block = false;
        }
        else {
            char* key = token;
            char* value = strtok(NULL, " \n\t;");
            /*debug(__func__, "key: %s\nvalue: %s", key, value);*/

            if (value) {
                if (http_block && !route_block) {
                    if (strcmp(key, "port:") == 0) srv.port = atoi(value);
                    else if (strcmp(key, "web_root:") == 0) srv.web_root = strdup(value);
                    debug(__func__, "http was updated:\nport: %d\nweb_root: %s", srv.port, srv.web_root);
                } 
                else if (route_block) {
                    if (strcmp(key, "mapping:") == 0) current_route.mapping = strdup(value);
                    else if (strcmp(key, "file_path:") == 0) current_route.file_path = strdup(value);
                    else if (strcmp(key, "web_root:") == 0) current_route.web_root = strdup(value);
                    debug(__func__, "route was updated:\nmapping: %s\nfile_path: %s\nweb_root: %s", current_route.mapping, current_route.file_path, current_route.web_root);
                }
            }
        }
        token = strtok(NULL, " \n\t");
    }
    
    free(file_content);
    return srv;
}
