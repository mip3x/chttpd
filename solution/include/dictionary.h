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

// TODO: implement iteration through hashtable

void free_dictionary();

#endif
