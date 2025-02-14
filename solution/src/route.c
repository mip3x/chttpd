#include "../include/route.h"
#include "../include/common.h"

void print_route(route route) {
    debug(__func__, 
          "route:\n\tweb_root: %s\n\tmapping: %s\n\tfile_path: %s",
            route.web_root,
            route.mapping,
            route.file_path
    );
}
