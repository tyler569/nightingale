#include <basic.h>
#include <assert.h>
#include <stdio.h>
#include <limine.h>

__MUST_EMIT
static struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0,
};

void limine_memmap() {
    assert(memmap_request.response);
    for (int i = 0; i < memmap_request.response->entry_count; i++) {
        struct limine_memmap_entry *entry = memmap_request.response->entries[i];

        printf("%lu %lx %lx", entry->type, entry->base, entry->length);
    }
}