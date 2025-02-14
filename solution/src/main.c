#include <stddef.h>

#include "../include/networking/server.h"
#include "../include/dictionary.h"
#include "../include/config_parser.h"
#include "../include/extractor.h"

int main() {
    char* config_file_path = DEFAULT_CONFIG_PATH;
    server srv = parse_config(config_file_path);

    if (srv.web_root) CONFIG_WEB_ROOT_PATH = srv.web_root;
    perform_action_on_each_member(set_config_web_root_if_needed);

    debug(__func__, "srv web_root: %s\n", srv.web_root);

    explore_routes();
    PRINT_ALL_ROUTES();

    status code = init_server(&srv, 0, NULL);
    if (code == ERROR) return ERROR;
    
    code = launch_server(&srv);
    if (code == ERROR) return ERROR;

    destroy_server(&srv);

    free_dictionary();
    return OK;
}
