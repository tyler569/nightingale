
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <multiboot.h>
#include <kernel/mem/vmm.h>

#include <kernel/printk.h> //debug



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
       return PAGE_NOT_PRESENT;
    }
    uintptr_t *pt = (uintptr_t *)0xFFC00000 + (0x400 * pdindex);

    if (! (pt[ptindex] & PAGE_PRESENT)) {
        return PAGE_NOT_PRESENT;
    }
    return (pt[ptindex] & PAGE_MASK) + (vma & PAGE_OFFSET);
}

/*
 * Allocate vma - does NOT map the page
 * Allocated pages are mapped the next time an access is attempted
 * on that virtual page.  This limits resident memory to memory 
 * being actually utilized.
 */
int vmm_alloc_page(uintptr_t vma, uint32_t flags) {
    klog("vmm_alloc_page: %08lx", vma);
    if (vmm_page_offset(vma) != 0) {
        klog("Alignment error");
        return PAGE_ALIGNMENT_ERROR;
    }
    
    if (! (*vmm_pd_entry(vma) & PAGE_PRESENT)) {
        klog("Attempting to alloc PT @ %08lx", vmm_pt(vma));
        vmm_alloc_page(vmm_pt(vma), flags);
    }

    if (*vmm_pt_entry(vma) & PAGE_PRESENT) {
        klog("Page @ %08lx already present", vma);
        return PAGE_ALREADY_PRESENT;
    }

    *vmm_pt_entry(vma) = flags | PAGE_ALLOCATED;
    return 0;
}
