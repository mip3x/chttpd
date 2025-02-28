#ifndef IO_H
#define IO_H

#include <stdlib.h>

#include "status.h"

status read_file(char** const file_content, const char* file_path, size_t* file_size);

#endif
