#include <regex.h>
#include <stddef.h>
#include <regex.h>
#include <stdio.h>
#include <string.h>

#include "../include/common.h"
#include "../include/dictionary.h"
#include "../include/extractor.h"
#include "../include/io.h"
#include "../include/route.h"

const char* patterns[] = {
    "<link[^>]+href=\"([^\"]+)\"",
    "<script[^>]+src=\"([^\"]+)\""
};

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

            char buf[BUFFER_SIZE];
            snprintf(buf, sizeof buf, "/%s", extracted_link);

            route extracted_link_route = {
                .web_root = strdup(incoming_route->web_root),
                .file_path = strdup(extracted_link),
                .mapping = strdup(buf),
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
