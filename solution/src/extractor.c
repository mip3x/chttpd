#include <regex.h>
#include <stddef.h>
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "../include/common.h"
#include "../include/dictionary.h"
#include "../include/extractor.h"
#include "../include/io.h"
#include "../include/route.h"
#include "../include/string_extension.h"

const char* patterns[] = {
    "<link[^>]+href=\"([^\"]+)\"",
    "<script[^>]+src=\"([^\"]+)\""
};

// wdid: count STEP_BACK_TRANSITION occurrences and cuts incoming_web_root's last transition
// let web_root be next: './www/web/root/number/one/'
// there will be '/' always at the end
// then, absolute path for './../../styles.css' will be found next way:
// ./../../styles.css <-> ./www/web/root/number/one/
// ./../styles.css <-> ./www/web/root/number/
// ./styles.css <-> ./www/web/root/
static char* get_absolute_path(char* relative_path, char* incoming_web_root) {
    bool has_nostep_prefix = starts_with(NOSTEP_TRANSITION, relative_path);

    char buf[BUFFER_SIZE];
    char* relative_path_with_prefix;

    if (!has_nostep_prefix) snprintf(buf, sizeof buf, "%s%s", NOSTEP_TRANSITION, relative_path); 
    else strcpy(buf, relative_path);

    relative_path_with_prefix = strdup(buf);
    debug(__func__, "relative_path_with_prefix: %s", relative_path_with_prefix);

    size_t step_back_occurrences_number = get_occurrences_number(relative_path_with_prefix, STEP_BACK_TRANSITION);

    debug(__func__, "number of occurrences: %zu\nweb_root: %s", step_back_occurrences_number, incoming_web_root);
    if (step_back_occurrences_number == 0) {
        char* absolute_path;
        asprintf(&absolute_path, "%s%s", incoming_web_root, relative_path);
        free(relative_path_with_prefix);
        return absolute_path;
    };

    char* tmp_web_root = strdup(incoming_web_root);
    tmp_web_root[strlen(tmp_web_root) - 1] = '\0';
    debug(__func__, "tmp_web_root start: %s", tmp_web_root);

    for (size_t i = 0; i < step_back_occurrences_number; i++) {
        char* last_slash = strrchr(tmp_web_root, SLASH_DELIMITER_BYTE);
        if (last_slash) *last_slash = '\0';
        else break;

        debug(__func__, "tmp_web_root: %s", tmp_web_root);
    }
    debug(__func__, "tmp_web_root final: %s", tmp_web_root);

    if (strlen(tmp_web_root) < strlen(incoming_web_root)) tmp_web_root = strdup(CONFIG_WEB_ROOT_PATH);
    else {
        snprintf(buf, sizeof buf, "%s/", tmp_web_root);
        strcpy(tmp_web_root, buf);
    }

    char* absolute_path;
    asprintf(&absolute_path, "%s%s", tmp_web_root, relative_path + (strlen(STEP_BACK_TRANSITION) - 1) * step_back_occurrences_number);

    free(tmp_web_root);
    free(relative_path_with_prefix);

    return absolute_path;
}

static void extract_links(route* incoming_route) {
    PRINT_ROUTE(*incoming_route);

    char* file_path_with_web_root = NULL;
    asprintf(&file_path_with_web_root, "%s%s", incoming_route->web_root, incoming_route->file_path);

    size_t file_size;
    char* file_content = read_file(file_path_with_web_root, &file_size);
    if (!file_content) {
        err("failed to read file %s\n", file_path_with_web_root);
        return;
    }

    regex_t preg;
    size_t patterns_length = sizeof(patterns) / sizeof(patterns[0]);

    const size_t nmatch = 2;
    regmatch_t pmatch[nmatch + 1];

    for (size_t i = 0; i < patterns_length; i++) {
        if (regcomp(&preg, patterns[i], REG_EXTENDED) != 0) {
            err("failed to compile pattern\n");
            continue;
        }

        char* search_start = file_content;
        while (regexec(&preg, search_start, nmatch, pmatch, 0) == 0) {
            regmatch_t extracted_link_regmatch_t = pmatch[1];
            size_t extracted_link_length = extracted_link_regmatch_t.rm_eo - extracted_link_regmatch_t.rm_so;
            char* extracted_link = strndup(search_start + extracted_link_regmatch_t.rm_so, extracted_link_length);

            debug(__func__, "extracted link: %s", extracted_link);
            char* absolute_path = get_absolute_path(extracted_link, incoming_route->web_root);

            debug(__func__, "absolute path: %s", absolute_path);

            /*char buf[BUFFER_SIZE];*/
            /*snprintf(buf, sizeof buf, "%s", relative_path);*/

            route extracted_link_route = {
                .web_root = strdup(incoming_route->web_root),
                .file_path = strdup(extracted_link),
                .mapping = strdup(absolute_path),
            }; 

            install(extracted_link_route);
            debug(__func__, "web_root + extracted_link path: %s", extracted_link_route.mapping);

            search_start += extracted_link_regmatch_t.rm_eo;
            free(extracted_link);
        }
    }

    regfree(&preg);
    free(file_content);
    free(file_path_with_web_root);
}

/*
  explore_routes() - explore routes for referenced files
*/
void explore_routes() {
    perform_action_on_each_member(extract_links);
}
