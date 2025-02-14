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
        if (strcmp(s, np->entry.mapping) == 0)
            return np;
    return NULL;
}

struct nlist* install(route entry) {
    struct nlist* np;
    uint32_t hashval;
    if ((np = lookup(entry.mapping)) == NULL) {
        np = (struct nlist*) malloc(sizeof(*np));
        if (np == NULL || (np->entry.mapping = strdup(entry.mapping)) == NULL)
            return NULL;
        hashval = hash(entry.mapping);
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
            free(temp->entry.mapping);
            free(temp->entry.file_path);
            free(temp->entry.web_root);
            np = np->next;
            free(temp);
        }
        hashtab[i] = NULL;
    }
}
