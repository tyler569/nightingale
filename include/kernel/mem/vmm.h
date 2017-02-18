
#pragma once

#include <stddef.h>
#include <multiboot.h>

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
#define PAGE_ALIGNMENT_ERROR -1
#define PAGE_NOT_PRESENT     -2
#define PAGE_ALREADY_PRESENT -3

#define PAGE_PRESENT        (1<<0)
#define PAGE_WRITE_ENABLE   (1<<1)
#define PAGE_USER_MODE      (1<<2)
#define PAGE_WRITE_THROUGH  (1<<3)
#define PAGE_CACHE_DISABLE  (1<<4)
#define PAGE_ACCESSED       (1<<5)
#define PAGE_DIRTY          (1<<6)
#define PAGE_MAPS_4M        (1<<7)
#define PAGE_GLOBAL         (1<<8)
#define PAGE_ALLOCATED      (1<<9)

#define PAGE_OFFSET  0xFFF
#define PAGE_MASK    (~PAGE_OFFSET)

/* I use a recureive page directory mapping, at position 1023 */
#define PAGE_DIRECTORY  0xFFFFF000
#define PAGE_TABLES     0xFFC00000
uintptr_t vma_to_pma(uintptr_t vma);

static inline uintptr_t vmm_pd_ix(uintptr_t vma) {
    return vma >> 22;
}

static inline uintptr_t vmm_pt_ix(uintptr_t vma) {
    return (vma >> 12) & 0x3FF;
}

static inline uintptr_t vmm_page_offset(uintptr_t vma) {
    return vma & PAGE_OFFSET;
}

static inline uintptr_t *vmm_pd_entry(uintptr_t vma) {
    return (uintptr_t *)PAGE_DIRECTORY + vmm_pd_ix(vma);
}

static inline uintptr_t *vmm_pt(uintptr_t vma) {
    return (uintptr_t *)PAGE_TABLES + (vmm_pd_ix(vma) * 1024);
}

static inline uintptr_t *vmm_pt_entry(uintptr_t vma) {
    return vmm_pt(vma) + vmm_pt_ix(vma);
}

int vmm_alloc_page(uintptr_t vma, uint32_t flags);

