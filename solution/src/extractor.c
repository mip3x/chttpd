#include <regex.h>
#include <stddef.h>
#include <regex.h>
#include <stdio.h>
#include <string.h>

#include "../include/common.h"
#include "../include/io.h"
#include "../include/extractor.h"

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

void extract_links(const char* file_path, const char* web_root) {
    char* file_path_with_web_root = NULL;
    asprintf(&file_path_with_web_root, "%s%s", web_root, file_path);

    size_t file_size;
    char* file_content = read_file(file_path_with_web_root, &file_size);

    regex_t preg;
    size_t patterns_length = sizeof(patterns) / sizeof(patterns[0]);

    const size_t nmatch = 2;
    regmatch_t pmatch[nmatch + 1];

    char** extracted_links = malloc(REFERENCED_FILES * sizeof(char*));
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

    // TODO: install routes
    
    debug(__func__, "file_path_with_web_root: %s", file_path_with_web_root);
    free(file_path_with_web_root);
    free_extracted_links(extracted_links, extracted_links_count);
}

void process_routes() {
    // TODO: iterate through routes and extract links in each
}
