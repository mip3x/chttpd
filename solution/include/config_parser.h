#ifndef PARSER_H
#define PARSER_H

#include "./networking/server.h"

#define DEFAULT_CONFIG_PATH "server.conf"

server parse_config(const char* file_path);

#endif
