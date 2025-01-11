#include <stddef.h>

#include "../include/networking/server.h"
#include "../include/route.h"
#include "../include/dictionary.h"

int main() {
    route r1 = {.mapping = "/route1", .file_path = "file1"};
    route r2 = {.mapping = "/route2", .file_path = "file2"};
    route main = {.mapping = "/", .file_path = "nichts"};
    install(r1);
    install(r2);
    install(main);

    server srv;
    status code = init_server(&srv, 0, NULL);
    if (code == ERROR) return ERROR;
    
    code = launch_server(&srv);
    if (code == ERROR) return ERROR;

    destroy_server(&srv);
    return OK;
}
