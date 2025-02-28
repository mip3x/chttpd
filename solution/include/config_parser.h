#ifndef PARSER_H
#define PARSER_H

#include "./networking/server.h"

#define DEFAULT_CONFIG_PATH "server.conf"

status parse_config(server* srv, const char* file_path);

#endif
