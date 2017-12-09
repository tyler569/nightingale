
#include <stddef.h>
#include <stdint.h>
#include <debug.h>

#include "paging.h"

uintptr_t resolve_virtual_to_physical(uintptr_t virtual) {
    uintptr_t p4_addr;
    __asm__ __volatile__ ("movq %%cr3, %0;" : "=r" (p4_addr));

    DEBUG_PRINTF("Resolving: %p\n", virtual);
    DEBUG_PRINTF("CR3      : %p\n", p4_addr);

    size_t p4_offset = (virtual >> 39) & 0777;
    size_t p3_offset = (virtual >> 30) & 0777;
    size_t p2_offset = (virtual >> 21) & 0777;
    size_t p1_offset = (virtual >> 12) & 0777;

    DEBUG_PRINTF("p4_offset : %x\n", p4_offset);
    DEBUG_PRINTF("p3_offset : %x\n", p3_offset);
    DEBUG_PRINTF("p2_offset : %x\n", p2_offset);
    DEBUG_PRINTF("p1_offset : %x\n", p1_offset);

    uintptr_t p3_addr = ((uintptr_t *)p4_addr)[p4_offset];
    if (!(p3_addr & PAGE_PRESENT)) {
        return -1;
    }
    p3_addr &= ~PAGE_MASK_4K;

    uintptr_t p2_addr = ((uintptr_t *)p3_addr)[p3_offset];
    if (!(p2_addr & PAGE_PRESENT)) {
        return -1;
    }
    if (p2_addr & PAGE_ISHUGE) {
        return (p2_addr & ~PAGE_MASK_1G) + (virtual & PAGE_MASK_1G);
    }
    p2_addr &= ~PAGE_MASK_4K;

    uintptr_t p1_addr = ((uintptr_t *)p2_addr)[p2_offset];
    if (!(p1_addr & PAGE_PRESENT)) {
        return -1;
    }
    if (p1_addr & PAGE_ISHUGE) {
        return (p1_addr & ~PAGE_MASK_2M) + (virtual & PAGE_MASK_2M);
    }
    p1_addr &= ~PAGE_MASK_4K;

    uintptr_t page_addr = ((uintptr_t *)p1_addr)[p1_offset];
    if (!(page_addr & PAGE_PRESENT)) {
        return -1;
    }

    return (page_addr & ~PAGE_MASK_4K) + (virtual & PAGE_MASK_4K);
}

