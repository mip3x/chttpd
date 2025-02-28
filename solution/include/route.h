#ifndef ROUTE_H
#define ROUTE_H

#include <regex.h>

#include "common.h"
#include "networking/server.h"

extern char* CONFIG_WEB_ROOT_PATH;

typedef enum {
    STRING,
    REGEX
} data_type;

static const char * const mapping_data_types[] = {
	[STRING] = "STRING",
	[REGEX] = "REGEX"
};

typedef struct {
    char* pattern;
    regex_t preg;
} regular_expression;

typedef struct {
    data_type type;
    union {
        char* string;
        regular_expression regex;
    } data;
} mapping;

typedef struct {
    char* web_root;
    mapping mapping;
    char* file_path;
} route;

#define PRINT_ROUTE(route) \
    debug(__func__, "PRINT_ROUTE::\nroute:\n\tweb_root: %s\n\tmapping_type: %s\n\tmapping_value: %s\n\tfile_path: %s", \
        (route).web_root, \
        mapping_data_types[(route).mapping.type], \
        ((route).mapping.type == STRING ? (route).mapping.data.string : (route).mapping.data.regex.pattern), \
        (route).file_path \
    );

void print_route(route* route);
void set_config_web_root_if_needed(route* route);

#endif
