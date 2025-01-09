#include <stdint.h>
#include "route.h"

struct nlist {
    struct nlist* next;
    route entry;
};

#define HASHSIZE 101
static struct nlist* hashtab[HASHSIZE];

uint32_t hash(char* s);

struct nlist* lookup(char* s);
struct nlist* install(route entry);
