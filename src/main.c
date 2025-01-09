#include <stddef.h>
#include "../include/networking/server.h"

int main() {
    server srv;
    status code = init_server(&srv, 0, NULL);
    if (code == ERROR) return ERROR;
    
    code = launch_server(&srv);
    if (code == ERROR) return ERROR;

    destroy_server(&srv);
    return OK;
}
