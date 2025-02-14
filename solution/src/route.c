#include <string.h>

#include "../include/route.h"

char* CONFIG_WEB_ROOT_PATH = NULL;

void print_route(route* route) {
    PRINT_ROUTE(*route);
}

void set_config_web_root_if_needed(route* route) {
    if (strcmp(route->web_root, DEFAULT_WEB_ROOT_PATH) == 0) route->web_root = CONFIG_WEB_ROOT_PATH;
}
