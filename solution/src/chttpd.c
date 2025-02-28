#include <stddef.h>

#include "../include/networking/server.h"
#include "../include/dictionary.h"
#include "../include/config_parser.h"
#include "../include/extractor.h"
#include "../include/404.h"

#define HANDLE_ERROR_STATUS(code) if ((code) != OK) { print_err_message_by_status(code); return (code); }

int main(int argc, char* argv[]) {
    char* config_file_path = DEFAULT_CONFIG_PATH;

    if (argc == 2) config_file_path = argv[1];
    server srv = {0};
    status parsed = parse_config(&srv, config_file_path);
    HANDLE_ERROR_STATUS(parsed);

    if (srv.web_root) CONFIG_WEB_ROOT_PATH = srv.web_root;
    perform_action_on_each_member(set_config_web_root_if_needed);

    debug(__func__, "srv web_root: %s\n", srv.web_root);

    explore_routes();
    install_404_route();
    PRINT_ALL_ROUTES();

    status initialized = init_server(&srv, 0, NULL);
    HANDLE_ERROR_STATUS(initialized);

    status launched = launch_server(&srv);
    HANDLE_ERROR_STATUS(launched);

    destroy_server(&srv);

    free_dictionary();
    return OK;
}
