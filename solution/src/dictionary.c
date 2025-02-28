#include <regex.h>
#include <stdlib.h>
#include <string.h>

#include "../include/dictionary.h"

static struct nlist* hashtab[HASHSIZE];

uint32_t hash(char* s) {
    uint32_t hashval;
    for (hashval = 0; *s != '\0'; s++)
        hashval = *s + 31 * hashval;
    return hashval % HASHSIZE;
}

struct nlist* lookup(char* s) {
    struct nlist* np;

    for (np = hashtab[hash(s)]; np != NULL; np = np->next)
        if (np->entry.mapping.type == STRING && strcmp(np->entry.mapping.data.string, s) == 0)
            return np;

    for (np = hashtab[0]; np != NULL; np = np->next)
        if (np->entry.mapping.type == REGEX && regexec(&np->entry.mapping.data.regex.preg, s, 0, NULL, 0) == 0)
            return np;

    return NULL;
}

struct nlist* install(route entry) {
    struct nlist* np;
    uint32_t hashval;

    if (entry.mapping.type == STRING) hashval = hash(entry.mapping.data.string);
    else hashval = 0; // bucket 0 for REGEX

    np = lookup( (entry.mapping.type == STRING) ? entry.mapping.data.string : entry.mapping.data.regex.pattern ); 
    if (np == NULL) {
        np = (struct nlist*) malloc(sizeof(*np));
        if (np == NULL) return NULL;

        np->entry.mapping.type = entry.mapping.type;

        np->next = hashtab[hashval];
        hashtab[hashval] = np;
    } else
        free((void*)np->entry.file_path);

    if ((np->entry.file_path = strdup(entry.file_path)) == NULL)
        return NULL;
    if (entry.web_root != NULL)
        np->entry.web_root = strdup(entry.web_root);
    else 
        np->entry.web_root = DEFAULT_WEB_ROOT_PATH;

    if (entry.mapping.type == STRING) np->entry.mapping.data.string = strdup(entry.mapping.data.string);
    else {
        np->entry.mapping.data.regex.pattern = strdup(entry.mapping.data.regex.pattern);
        memcpy(&np->entry.mapping.data.regex.preg, &entry.mapping.data.regex.preg, sizeof(regex_t));
    }

    return np;
}

void perform_action_on_each_member(void (*func)(route*)) {
    for (int i = 0; i < HASHSIZE; i++) {
        struct nlist* np = hashtab[i];
        while (np != NULL) {
            func(&np->entry);
            np = np->next;
        }
    }
}

void free_dictionary() {
    for (int i = 0; i < HASHSIZE; i++) {
        struct nlist* np = hashtab[i];
        while (np != NULL) {
            struct nlist* temp = np;
            if (temp->entry.mapping.type == REGEX) {
                regfree(&temp->entry.mapping.data.regex.preg);
                free(temp->entry.mapping.data.regex.pattern);
            }
            else free(temp->entry.mapping.data.string);
            free(temp->entry.file_path);
            free(temp->entry.web_root);
            np = np->next;
            free(temp);
        }
        hashtab[i] = NULL;
    }
}
