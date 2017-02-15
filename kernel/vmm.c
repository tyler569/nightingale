
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <multiboot.h>
#include <kernel/memory.h>

#include <kernel/printk.h> //debug

void memory_init(multiboot_info_t *mbdata) {
    if (!(mbdata->flags & MULTIBOOT_INFO_MEM_MAP)) {
        klog("No memory map data available");
        abort();
    }

    // memset(physical_memory_map, 0, 1024);

    multiboot_memory_map_t *mbmap = (multiboot_memory_map_t *)mbdata->mmap_addr;
    size_t mbmap_max = mbdata->mmap_length / sizeof(multiboot_memory_map_t);

    uintptr_t start, end;

    for (size_t i = 0; i < mbmap_max; i++) {
        printk("\t%08llx (%08llx)\n", (mbmap+i)->addr, (mbmap+i)->len);
        // 4GB only, no PSE or PAE.  Type must be available (1)
        if ((mbmap+i)->addr > 0xFFFFFFFF || (mbmap+i)->type != 1) {
            continue;
        }

        start = ALIGN4M((mbmap+i)->addr);
        end = start + (mbmap+i)->len;
        while (end - start > 0x400000) {
            // phyindex = start >> 22;
            // physical_memory_map[phyindex] |= PHY_PAGE_EXISTS;
            start += 0x400000;
        }
        // First page mapped to 0xc0000000 in boot.S
        // physical_memory_map[0] |= PHY_PAGE_MAPPED; 
    }
    kmalloc_init();
}

/*
 * If you call this function on an unmapped vma
 * you are not guaranteed to get sensible results
 *  - for now, you can roll the dice, 1/4B that you
 * *actually* got (T *)-1, or (more likely) it's not valid.
 * See note in include/kernel/kmem.h .
 */
uintptr_t vma_to_pma(uintptr_t vma) {
    uintptr_t *pd = (uintptr_t *)0xFFFFF000;
    uintptr_t pdindex = vma >> 22;
    uintptr_t ptindex = vma >> 12 & 0x3FF;

    if (! (pd[pdindex] & PAGE_PRESENT)) {
       return ERROR_NOT_PRESENT;
    }
    if (! (pd[pdindex] & PAGE_MAPS_4M)) { // 4k pages
        uintptr_t *pt = (uintptr_t *)(pd[pdindex] & (0xFFC00000 + ptindex));

        if (! (pt[ptindex] & PAGE_PRESENT)) {
            return ERROR_NOT_PRESENT;
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
    uintptr_t *pd = (uintptr_t *)0xFFFFF000;
    if ((pma & PAGE_4M_OFFSET) != 0 || (vma & PAGE_4M_OFFSET) != 0) {
        return PAGE_ALIGNMENT_ERROR;
    }

    uintptr_t pdindex = vma >> 22;

    if (pd[pdindex] & PAGE_PRESENT) {
        return PAGE_ALREADY_PRESENT;
    }

    pd[pdindex] = pma | 0x83; // 4M page, write enable, present
    // physical_memory_map[pma >> 22] |= PHY_PAGE_MAPPED;
    return 0;
}

int alloc_page(uintptr_t vma) {
    return OUT_OF_MEMORY;
}
/*
int map_4k_page(uintptr_t *pd, uintptr_t vma, uintptr_t pma) {
    if ((pma & PAGE_4K_OFFSET) != 0 || (vma & PAGE_4K_OFFSET) != 0) {
        return PAGE_ALIGNMENT_ERROR;
    }

    uintptr_t 


int alloc_4M_page(uintptr_t vma) {
    uintptr_t *pd = (uintptr_t *)0xFFFFF000;
    if ((vma & PAGE_4M_OFFSET) != 0) {
        return PAGE_ALIGNMENT_ERROR;
    }

    for (size_t i = 0; i < PHY_PAGE_MAP_LEN; i++) {
        if ( (physical_memory_map[i] & PHY_PAGE_EXISTS) &&
            !(physical_memory_map[i] & PHY_PAGE_MAPPED)) {
                return map_4M_page(pd, vma, i << 22);
        }
    } 

    return OUT_OF_MEMORY;
} */

int unmap_page(uintptr_t vma) {
    uintptr_t *pd = (uintptr_t *)0xFFFFF000;
    if ((vma & PAGE_4K_OFFSET) != 0) {
        return PAGE_ALIGNMENT_ERROR;
    }
    
    uintptr_t pdindex = vma >> 22;

    if (! (pd[pdindex] & PAGE_PRESENT)) {
        return PAGE_ALREADY_VACANT;
    }

    uintptr_t ptindex = vma >> 12 & 0x3FF;
    uintptr_t *pt = (uintptr_t *)(pd[pdindex] & (0xFFC00000 + ptindex));
    pt[ptindex] &= ~PAGE_PRESENT;

    return 0;
}

