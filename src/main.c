#include <stddef.h>

#include "../include/networking/server.h"
#include "../include/route.h"
#include "../include/dictionary.h"

int main() {
    route r1 = {.mapping = "/route1", .file_path = "file1"};
    route r2 = {.mapping = "/route2", .file_path = "file2"};
    route r3 = {.mapping = "/main", .file_path = "index.html"};
    route r4 = {.mapping = "/styles.mrcds", .file_path = "styles.mrcds"};
    install(r1);
    install(r2);
    install(r3);
    install(r4);

    server srv;
    status code = init_server(&srv, 0, NULL);
    if (code == ERROR) return ERROR;
    
    code = launch_server(&srv);
    if (code == ERROR) return ERROR;

    destroy_server(&srv);
    return OK;
}
