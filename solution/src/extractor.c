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

static void free_extracted_links(char** const extracted_links, size_t count) {
    if (!extracted_links) return;

    for (size_t i = 0; i < count; i++)
        if (extracted_links[i]) free(extracted_links[i]);
    free(extracted_links);
}

static void extract_links(route* route) {
    char** extracted_links = malloc(REFERENCED_FILES * sizeof(char*));
    if (!extracted_links) {
        err("failed to allocate memory for extracted_links\n");
        return;
    }

    PRINT_ROUTE(*route);

    char* file_path_with_web_root = NULL;
    asprintf(&file_path_with_web_root, "%s%s", route->web_root, route->file_path);

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

    size_t extracted_links_count = 0;

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

            extracted_links[extracted_links_count++] = extracted_link;
            debug(__func__, "extracted link: %s", extracted_link);

            search_start += extracted_link_regmatch_t.rm_eo;
        }
    }

    regfree(&preg);
    free(file_content);

    debug(__func__, "file_path_with_web_root: %s", file_path_with_web_root);

    // TODO: install routes
    
    free(file_path_with_web_root);
    free_extracted_links(extracted_links, extracted_links_count);
}

/*
  explore_routes() - explore routes for referenced files
*/
void explore_routes() {
    perform_action_on_each_member(extract_links);
}
