
#pragma once

#include <stddef.h>
#include <multiboot.h>

#define PHY_PAGE_MAP_LEN 1024

#define PHY_PAGE_EXISTS 0x01
#define PHY_PAGE_MAPPED 0x02

#define ALIGN4M(x) ((x + 0x3FFFFF) & ~0x3FFFFF)

void memory_init(multiboot_info_t *mbdata);

int paging_mem_available();

/*
 * Note that techncically, -1 can be a valid physical memory address that
 * returns from a mapping, but I'm going with this for 2 reasons:
 * 1. it's easy to check if the offset (lower 12/22 bits) are all 1's,
 *    in which case it could actutually be valid, but so would address - 1
 *    and that would not return -1
 *      i.e. 0xabcdefff -> 0xffffffff;
 *           0xabcdeffe -> 0xfffffffe; necessarily.
 *      In an unmapped page that would also return -1, as vould any other
 *      offset.
 * 2. this is the alternative to making a vma_is_mapped function, effectively
 *    requiring me to trawl the same pages twice per lookup (once to get the 
 *    Present bit, then return that and do the same lookups again
 */
#define ERROR_NOT_PRESENT -1

#define PAGE_PRESENT    0x01
#define PAGE_MAPS_4M    0x80
#define PAGE_4M_OFFSET  0x3FFFFF
#define PAGE_4M_MASK    (~PAGE_4M_OFFSET)
#define PAGE_4K_OFFSET  0xFFF
#define PAGE_4K_MASK    (~PAGE_4K_OFFSET)

uintptr_t vma_to_pma(uintptr_t vma);

/*
 *  
 */

#define PAGE_ALIGNMENT_ERROR -1
#define PAGE_ALREADY_PRESENT -2
#define PAGE_ALREADY_VACANT  -3

int map_4M_page(uintptr_t vma, uintptr_t pma);
int unmap_page(uintptr_t vma);

/* Maps a page to an arbitrary physical location */

#define OUT_OF_MEMORY -4
int alloc_page(uintptr_t vma);

/* ************************************************************************ */

void kmalloc_init();

void *kmalloc(size_t size);
void kfree(void *memory);


