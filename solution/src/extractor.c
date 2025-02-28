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

// wdid: count STEP_BACK_TRANSITION occurrences and cuts incoming_mapping's last transition
// let mapping be next: '/m1/m2/m3/m4'
// then, mapping for '../../styles.css' will be found next way:
// ../../styles.css <-> /m1/m2/m3/
// ../styles.css <-> /m1/m2/
// styles.css <-> /m1/
//
static char* get_mapping(char* relative_path, char* parent_mapping) {
    size_t step_back_occurrences_number = get_occurrences_number(relative_path, STEP_BACK_TRANSITION);

    debug(__func__, "number of occurrences: %zu\nparent_mapping: %s", step_back_occurrences_number, parent_mapping);
    char* tmp_parent_mapping = strdup(parent_mapping);

    for (size_t i = 0; i < step_back_occurrences_number + 1; i++) {
        char* last_slash = strrchr(tmp_parent_mapping, SLASH_DELIMITER);
        if (last_slash) *last_slash = '\0';
        else break;
    }

    char* child_mapping;
    asprintf(&child_mapping, "%s/%s", tmp_parent_mapping, relative_path + strlen(STEP_BACK_TRANSITION) * step_back_occurrences_number);
    free(tmp_parent_mapping);

    debug(__func__, "child_mapping: %s", child_mapping);
    return child_mapping;
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
            if (incoming_route->mapping.type == REGEX) {
                free(extracted_link);
                break;
            }

            char* mapping = get_mapping(extracted_link, incoming_route->mapping.data.string);

            debug(__func__, "mapping: %s", mapping);

            route extracted_link_route = {
                .web_root = strdup(incoming_route->web_root),
                .file_path = strdup(extracted_link),
                .mapping.type = STRING,
                .mapping.data.string = strdup(mapping),
            }; 

            install(extracted_link_route);
            debug(__func__, "web_root + extracted_link path: %s", extracted_link_route.mapping.data.string);

            search_start += extracted_link_regmatch_t.rm_eo;
            free(extracted_link);
            free(mapping);
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
