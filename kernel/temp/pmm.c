
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <multiboot.h>
#include <kernel/cpu.h>
#include <kernel/mem/pmm.h>
#include <kernel/mem/vmm.h>
#include <kernel/printk.h>

extern uintptr_t kernel_end;

uintptr_t *pmm_sp = PMM_STACK_TOP;



static uintptr_t pmm_pop_page() {
    if (pmm_sp == PMM_STACK_TOP) {
        klog("PMM OOM");
        panic();
    }
    return *(pmm_sp++);
}

static void pmm_push_page(uintptr_t pma) {
    pmm_sp--;
    if (vmm_page_offset((uintptr_t)pmm_sp) == 0) {
        pmm_raw_map_page(((uintptr_t)pmm_sp) - 0x1000, pma, PAGE_WRITE_ENABLE | PAGE_GLOBAL);
    } else {
        *pmm_sp = pma; 
    }
}

int pmm_raw_map_page(uintptr_t vma, uintptr_t pma, uint32_t flags) {
    if (vmm_page_offset(vma) != 0 || vmm_page_offset(pma) != 0) {
        return PAGE_ALIGNMENT_ERROR;
    }
    if (! (*vmm_pd_entry(vma) & PAGE_PRESENT)) {
        pmm_raw_map_page((uintptr_t)vmm_pt(vma), pmm_pop_page(), flags);
    }
    *vmm_pt_entry(vma) = pma | flags | PAGE_PRESENT;
    return 0;
}

int pmm_free_page(uintptr_t pma) {
    if ((pma & PAGE_OFFSET) != 0) {
        return PAGE_ALIGNMENT_ERROR;
    }
    pmm_push_page(pma);
}

void pmm_do_page_fault(struct regs *r) {
    uintptr_t cr2;
    __asm__ ( "movl %%cr2, %0": "=r" (cr2) );
    klog("#PF @ %08x . CR2: %08lx . ERR: %x", r->eip, cr2, r->err_code);

    cr2 &= PAGE_MASK;
    
    if (cr2 < 0x80000000) {
        klog("Mapping user mode space is not yet supported");
        panic();
    }
    //TODO: pf flags checking

    if (! (*vmm_pd_entry(cr2) & PAGE_PRESENT)) {
        klog("#PF in unallocated memory");
        panic();
    }

    if (! (*vmm_pt_entry(cr2) & PAGE_ALLOCATED)) {
        klog("#PF in unallocated memory");
        panic();
    }

    *vmm_pt_entry(cr2) &= PAGE_OFFSET;
    *vmm_pt_entry(cr2) |= pmm_pop_page();
    *vmm_pt_entry(cr2) |= PAGE_PRESENT;
    
    printk("#PF mapped %08lx to PHY:%08lx\n", cr2, *vmm_pt_entry(cr2));
} 

void pmm_init(multiboot_info_t *mbdata) {
    if (!(mbdata->flags & MULTIBOOT_INFO_MEM_MAP)) {
        klog("No memory map data available");
        abort();
    }

    multiboot_memory_map_t *mbmap = (multiboot_memory_map_t *)mbdata->mmap_addr;
    size_t mbmap_max = mbdata->mmap_length / sizeof(multiboot_memory_map_t);

    for (size_t i = 0; i < mbmap_max; i++) {
        printk("\t%08llx (%08llx) - %i\n", (mbmap+i)->addr, (mbmap+i)->len, (mbmap+i)->type);
       
        if ((mbmap+i)->type != 1) {
           continue;
        } 
        if ((mbmap+i)->addr > 0xFFFFFFFF) {
            continue;
        }
        for (uintptr_t page = (mbmap+i)->addr; page < (mbmap+i)->addr + (mbmap+i)->len; page += 0x1000) {
            //TODO: ensure all these are full pages
            if (page < (uintptr_t)&kernel_end - 0x7FF00000) {
                continue;
            }
            pmm_free_page(page);
        }
    }
    idt_install_handler(14, pmm_do_page_fault);
}

