#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <stdint.h>
#include "route.h"

struct nlist {
    struct nlist* next;
    route entry;
};

#define HASHSIZE 101

uint32_t hash(char* s);

struct nlist* lookup(char* s);
struct nlist* install(route entry);
void perform_action_on_each_member(void (*func)(route));

#define PRINT_ALL_ROUTES() debug(__func__, "PRINTING ALL ROUTES:\n");\
    perform_action_on_each_member(print_route);\
    debug(__func__, "\n");

void free_dictionary();

#endif
