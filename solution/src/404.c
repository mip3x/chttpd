#include "../include/dictionary.h"
#include "../include/route.h"
#include "../include/404.h"

status install_404_route() {
    route route404 = {
        .web_root = DEFAULT_WEB_ROOT_PATH,
        .file_path = DEFAULT_404_STYLES_FILE_PATH,
        .mapping = DEFAULT_404_STYLES_MAPPING
    };

    install(route404);

    return OK;
}
