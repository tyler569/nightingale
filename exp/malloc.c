
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

/*
 * A testbed in normal *nix userspace for a new
 * implementation of malloc/realloc/free for nightingale.
 *
 * Goals:
 *  - support alligned alloc up to 1 page (4096B)
 *  - efficiency (memory and time)
 */

char *memory;

void *n_malloc(size_t len);
void *n_calloc(size_t len, size_t count);
void *n_realloc(void *mem, size_t len);
void *n_free(void *mem);

struct mem_arena {
    size_t arena_len;
    size_t arena_asize;
    
};

int main() {
    memory = malloc(128 * 1024 * 1024);


}
