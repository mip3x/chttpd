#include "../include/server.h"

int main() {
    server* srv = init(SERVER_PORT, WEB_ROOT_PATH);
    status finish_code = start(srv);
    destroy(srv);

    return finish_code;
}
