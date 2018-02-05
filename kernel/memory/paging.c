
#include <basic.h>

#define DEBUG
#include <debug.h>

#include "paging.h"

//
// More consistent naming for the page tables
//
// P4 = PML4
// P3 = PDPT
// P2 = PD
// P1 = PT
//

// Work on recursive page mapping
//
// Actal mapping as it is now:
// ffffff8000000000: 0000000000110000 p1[0]
// ffffff8000001000: 0000000000111000 p1[1]
// ffffffffc0000000: 000000000010f000 p2
// ffffffffffe00000: 000000000010e000 p3
// fffffffffffff000: 000000000010d000 p4

#define P4_BASE 0xFFFFFFFFFFFFF000
#define P3_BASE 0xFFFFFFFFFFE00000
#define P2_BASE 0xFFFFFFFFC0000000
#define P1_BASE 0xFFFFFF8000000000

#define P3_OFFSET 

usize *get_page_table_root() {
    usize *p4_addr;
    asm volatile ("movq %%cr3, %0;" : "=r" (p4_addr));
    return p4_addr;
}

usize resolve_virtual_to_physical(usize virtual) {
    usize *p4_addr = get_page_table_root();

    DEBUG_PRINTF("Resolving: %p\n", virtual);
    DEBUG_PRINTF("CR3      : %p\n", p4_addr);

    usize p4_offset = (virtual >> 39) & 0777;
    usize p3_offset = (virtual >> 30) & 0777;
    usize p2_offset = (virtual >> 21) & 0777;
    usize p1_offset = (virtual >> 12) & 0777;

    DEBUG_PRINTF("p4_offset : %x\n", p4_offset);
    DEBUG_PRINTF("p3_offset : %x\n", p3_offset);
    DEBUG_PRINTF("p2_offset : %x\n", p2_offset);
    DEBUG_PRINTF("p1_offset : %x\n", p1_offset);

    usize p3_addr = (p4_addr)[p4_offset];
    if (!(p3_addr & PAGE_PRESENT)) {
        return -1;
    }
    p3_addr &= ~PAGE_MASK_4K;

    usize p2_addr = ((usize *)p3_addr)[p3_offset];
    if (!(p2_addr & PAGE_PRESENT)) {
        return -1;
    }
    if (p2_addr & PAGE_ISHUGE) {
        return (p2_addr & ~PAGE_MASK_1G) + (virtual & PAGE_MASK_1G);
    }
    p2_addr &= ~PAGE_MASK_4K;

    usize p1_addr = ((usize *)p2_addr)[p2_offset];
    if (!(p1_addr & PAGE_PRESENT)) {
        return -1;
    }
    if (p1_addr & PAGE_ISHUGE) {
        return (p1_addr & ~PAGE_MASK_2M) + (virtual & PAGE_MASK_2M);
    }
    p1_addr &= ~PAGE_MASK_4K;

    usize page_addr = ((usize *)p1_addr)[p1_offset];
    if (!(page_addr & PAGE_PRESENT)) {
        return -1;
    }

    return (page_addr & ~PAGE_MASK_4K) + (virtual & PAGE_MASK_4K);
}

bool allocate_map_p3(usize virtual) { return false; }
bool allocate_map_p2(usize virtual) { return false; }
bool allocate_map_p1(usize virtual) { return false; }

bool map_virtual_to_physical(usize virtual, usize physical) {
    // TODO: sizes other than 4k?
    physical &= ~PAGE_MASK_4K;

    DEBUG_PRINTF("Trying to map vma:%p to pma:%p\n", virtual, physical);

    usize *p4_addr = get_page_table_root();

    usize p4_offset = (virtual >> 39) & 0777;
    usize p3_offset = (virtual >> 30) & 0777;
    usize p2_offset = (virtual >> 21) & 0777;
    usize p1_offset = (virtual >> 12) & 0777;

    DEBUG_PRINTF("p4_offset : %x\n", p4_offset);
    DEBUG_PRINTF("p3_offset : %x\n", p3_offset);
    DEBUG_PRINTF("p2_offset : %x\n", p2_offset);
    DEBUG_PRINTF("p1_offset : %x\n", p1_offset);

    usize p3_addr = ((usize *)p4_addr)[p4_offset];
    if (!(p3_addr & PAGE_PRESENT)) {
        // TODO allocate/map
        WARN_PRINTF("Failing to map for unmapped P3 offset\n");
        return false;
    }
    p3_addr &= ~PAGE_MASK_4K;

    usize p2_addr = ((usize *)p3_addr)[p3_offset];
    if (!(p2_addr & PAGE_PRESENT)) {
        // TODO allocate/map
        WARN_PRINTF("Failing to map for unmapped P2 offset\n");
        return false;
    }
    if (p2_addr & PAGE_ISHUGE) {
        return (p2_addr & ~PAGE_MASK_1G) + (virtual & PAGE_MASK_1G);
    }
    p2_addr &= ~PAGE_MASK_4K;

    usize p1_addr = ((usize *)p2_addr)[p2_offset];
    if (!(p1_addr & PAGE_PRESENT)) {
        // TODO allocate/map
        WARN_PRINTF("Failing to map for unmapped P1 offset\n");
        return false;
    }
    if (p1_addr & PAGE_ISHUGE) {
        return (p1_addr & ~PAGE_MASK_2M) + (virtual & PAGE_MASK_2M);
    }
    p1_addr &= ~PAGE_MASK_4K;

    usize page_addr = ((usize *)p1_addr)[p1_offset];
    if (page_addr & PAGE_PRESENT) {
        // already mapped!
        WARN_PRINTF("Failing to map for page already mapped\n");
        return false;
    }

    DEBUG_PRINTF("Got a P1 at %p\n", p1_addr);
    DEBUG_PRINTF("Putting %p there\n", physical | PAGE_PRESENT | PAGE_WRITEABLE);
    // TODO: control these settings!
    ((usize *)p1_addr)[p1_offset] = physical | PAGE_PRESENT | PAGE_WRITEABLE;
    return true;
}

