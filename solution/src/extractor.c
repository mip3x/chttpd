#include <regex.h>
#include <stddef.h>
#include <regex.h>
#include <stdio.h>
#include <string.h>

#include "../include/common.h"
#include "../include/io.h"

const char* patterns[] = {
    "<link[^>]+href=\"([^\"]+)\"",
    "<script[^>]+src=\"([^\"]+)\""
};

void extract_links(const char* file_path, const char* web_root) {
    char* file_path_with_web_root = NULL;
    asprintf(&file_path_with_web_root, "%s%s", web_root, file_path);

    size_t file_size;
    char* file_content = read_file(file_path_with_web_root, &file_size);

    regex_t preg;
    size_t patterns_length = sizeof(patterns) / sizeof(patterns[0]);

    const size_t nmatch = 2;
    regmatch_t pmatch[nmatch + 1];

    for (size_t i = 0; i < patterns_length; i++) {
        if (regcomp(&preg, patterns[i], REG_EXTENDED) != 0) {
            err("failed to compile pattern\n");
            continue;
        }

        int regexec_result = regexec(&preg, file_content, nmatch, pmatch, 0);

        if (regexec_result == 0) {
            debug(__func__, "match found in %s\n", file_path_with_web_root);

            regmatch_t extracted_link = pmatch[1];
            if (extracted_link.rm_so != -1) {
                char buf[256] = {0};
                strncpy(buf, file_content + extracted_link.rm_so, extracted_link.rm_eo - extracted_link.rm_so);
                debug(__func__, "start %d, end %d: %s\n", extracted_link.rm_so, extracted_link.rm_eo, buf);
            }
        }
        else if (regexec_result == REG_NOMATCH)
            debug(__func__, "no match found in %s\n", file_path_with_web_root);
    }

    regfree(&preg);
    free(file_content);

    // TODO: install routes
}

void process_routes() {
    // TODO: iterate through routes and extract links in each
}
