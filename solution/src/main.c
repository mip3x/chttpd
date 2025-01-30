#include <stddef.h>

#include "../include/networking/server.h"
#include "../include/route.h"
#include "../include/dictionary.h"
#include "../include/config_parser.h"

int main() {
    /*route r1 = {.mapping = "/route1", .file_path = "file1"};*/
    /*route r2 = {.mapping = "/route2", .file_path = "file2"};*/
    /*route r3 = {.mapping = "/", .file_path = "index.html"};*/
    /*route r4 = {.mapping = "/styles.mrcds", .file_path = "styles.mrcds"};*/
    /*route r5 = {.mapping = "/404.css", .file_path = "default/404.css"};*/
    /*route r6 = {.mapping = "/script.js", .file_path = "script.js"};*/
    /*route r7 = {.mapping = "/200", .file_path = "200.html", .web_root = "./www/subfolder/"};*/
    /*route r8 = {.mapping = "/styles/main.css", .file_path = "styles/main.css", .web_root = "./www/subfolder/"};*/
    /*install(r1);*/
    /*install(r2);*/
    /*install(r3);*/
    /*install(r4);*/
    /*install(r5);*/
    /*install(r6);*/
    /*install(r7);*/
    /*install(r8);*/

    char* config_file_path = DEFAULT_CONFIG_PATH;
    server srv = parse_config(config_file_path);

    /*server srv;*/
    status code = init_server(&srv, 0, NULL);
    if (code == ERROR) return ERROR;
    
    code = launch_server(&srv);
    if (code == ERROR) return ERROR;

    destroy_server(&srv);

    free_dictionary();
    return OK;
}
