
/* 
 */

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

#include <kernel/printk.h>
#include <kernel/kmem.h>

#include "multiboot.h"

struct physical_memory {
    uintptr_t base;
    uintptr_t length;
};

extern uintptr_t pagedir;
uintptr_t *pd = &pagedir;

int paging_mem_available() {
    /* Will return how much physical memory is available */
    return 0;
}

void paging_system_init(multiboot_info_t *mbdata) {
    if (! mbdata->flags & MULTIBOOT_INFO_MEM_MAP) {
        klog("No memory map data available");
        abort();
    }

    klog("Physical Memory Map:");

    multiboot_memory_map_t *mbmap = (multiboot_memory_map_t *)mbdata->mmap_addr;
    size_t mbmap_max = mbdata->mmap_length / sizeof(multiboot_memory_map_t);

    for (size_t i = 0; i < mbmap_max; i++) {
        printk("          Addr:%#08llx, ", (mbmap+i)->addr);
        printk("Len:%#08llx - ", (mbmap+i)->len);
        printk("%s\n", (mbmap+i)->type == 1 ? "Available" : "Unavailable");
    }
}


/*
 * If you call this function on an unmapped vma
 * you are not guaranteed to get sensible results
 *  - for now, you can roll the dice, 1/4G that you
 * *actually* got (T *)-1, or (more likely) it's not valid.
 */
uintptr_t vma_to_pma(uintptr_t vma) {
    uintptr_t pdindex = vma >> 22;
    uintptr_t ptindex = vma >> 12 & 0x3FF;

    if (! (pd[pdindex] & PAGE_PRESENT)) {
       return -1; // pd entry not present
    }
    if (! (pd[pdindex] & PAGE_MAPS_4M)) { // 4k pages
        uintptr_t *pt = (uintptr_t *)(pd[pdindex] & 0xFFC00000 + ptindex);

        if (! (pt[ptindex] & PAGE_PRESENT)) {
            return -1; // pt entry not present
        }
        return (pt[ptindex] & PAGE_4K_MASK) + (vma & PAGE_4K_OFFSET);
    } else { // 4M pages
        return (pd[pdindex] & PAGE_4M_MASK) + (vma & PAGE_4M_OFFSET);
    }

}

/*
 * TODO: Non-default flags
 */
int map_4M_page(uintptr_t vma, uintptr_t pma) {
    if (pma & PAGE_4M_OFFSET != 0 || vma & PAGE_4M_OFFSET != 0) {
        return PAGE_ALIGNMENT_ERROR;
    }

    uintptr_t pdindex = vma >> 22;

    if (pd[pdindex] & PAGE_PRESENT) {
        return PAGE_ALREADY_PRESENT;
    }

    pd[pdindex] = pma | 0x83; // 4M page, write enable, present 
    return 0;
}

int unmap_page(uintptr_t vma) {
    if (vma & PAGE_4K_OFFSET != 0) {
        return PAGE_ALIGNMENT_ERROR;
    }
    
    uintptr_t pdindex = vma >> 22;

    if (! (pd[pdindex] & PAGE_PRESENT)) {
        return PAGE_ALREADY_VACANT;
    }

    if (pd[pdindex] & PAGE_MAPS_4M) { // 4M pages
        if (vma & PAGE_4M_OFFSET != 0) {
            return PAGE_ALIGNMENT_ERROR;
        }

        pd[pdindex] &= ~PAGE_PRESENT; // clear present bit
    } else {                          // 4k pages
        uintptr_t ptindex = vma >> 12 & 0x3FF;
        uintptr_t *pt = (uintptr_t *)(pd[pdindex] & 0xFFC00000 + ptindex);
        pt[ptindex] &= ~PAGE_PRESENT;
    }

    return 0;
}

