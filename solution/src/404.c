#include "../include/dictionary.h"
#include "../include/route.h"
#include "../include/404.h"

status install_404_route() {
    route route404_styles = {
        .web_root = DEFAULT_WEB_ROOT_PATH,
        .file_path = DEFAULT_404_STYLES_FILE_PATH,
        .mapping.type = REGEX,
        .mapping.data.string = DEFAULT_404_STYLES_MAPPING
    };

    if (regcomp(&route404_styles.mapping.data.regex.preg, route404_styles.mapping.data.string, REG_EXTENDED) != 0)
        err("regular expression compilation error: %s", route404_styles.mapping.data.string);

    install(route404_styles);

    return OK;
}
