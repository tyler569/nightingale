
#include <basic.h>
#include <string.h>
#include <panic.h>
#include <malloc.h>
// #define DEBUG
#include <debug.h>
#include <arch/x86/cpu.h> // MOVE TO ARCH-DEPENDANT VMM BACKEND
#include "pmm.h"
#include "vmm.h"

//
// More consistent naming for the page tables
//
// P4 = PML4
// P3 = PDPT
// P2 = PD
// P1 = PT
//

#define __vmm_use_recursive

#ifdef __vmm_use_recursive

#define REC_ENTRY (uintptr_t)0400 // higher half + 0

#define P1_BASE (0xFFFF000000000000 + (REC_ENTRY << 39))
#define P2_BASE (P1_BASE + (REC_ENTRY << 30))
#define P3_BASE (P2_BASE + (REC_ENTRY << 21))
#define P4_BASE (P3_BASE + (REC_ENTRY << 12))

#define P1_STRIDE 0x1000
#define P2_STRIDE 0x200000
#define P3_STRIDE 0x40000000
#define SIZEOF_ENTRY sizeof(uintptr_t)

uintptr_t *vmm_get_p4_table(uintptr_t vma) {
    return (uintptr_t *)P4_BASE;
}

uintptr_t *vmm_get_p4_entry(uintptr_t vma) {
    uintptr_t p4_offset = (vma >> 39) & 0777;
    return (uintptr_t *)(P4_BASE + p4_offset * SIZEOF_ENTRY);
}

uintptr_t *vmm_get_p3_table(uintptr_t vma) {
    uintptr_t p4_offset = (vma >> 39) & 0777;
    return (uintptr_t *)(P3_BASE + p4_offset * P1_STRIDE);
}

uintptr_t *vmm_get_p3_entry(uintptr_t vma) {
    uintptr_t p4_offset = (vma >> 39) & 0777;
    uintptr_t p3_offset = (vma >> 30) & 0777;
    return (uintptr_t *)(P3_BASE + p4_offset * P1_STRIDE + p3_offset * SIZEOF_ENTRY);
}

uintptr_t *vmm_get_p2_table(uintptr_t vma) {
    uintptr_t p4_offset = (vma >> 39) & 0777;
    uintptr_t p3_offset = (vma >> 30) & 0777;
    return (uintptr_t *)(P2_BASE + p4_offset * P2_STRIDE + p3_offset * P1_STRIDE);
}

uintptr_t *vmm_get_p2_entry(uintptr_t vma) {
    uintptr_t p4_offset = (vma >> 39) & 0777;
    uintptr_t p3_offset = (vma >> 30) & 0777;
    uintptr_t p2_offset = (vma >> 21) & 0777;
    return (uintptr_t *)(P2_BASE + p4_offset * P2_STRIDE + p3_offset * P1_STRIDE + p2_offset * SIZEOF_ENTRY);
}

uintptr_t *vmm_get_p1_table(uintptr_t vma) {
    uintptr_t p4_offset = (vma >> 39) & 0777;
    uintptr_t p3_offset = (vma >> 30) & 0777;
    uintptr_t p2_offset = (vma >> 21) & 0777;
    return (uintptr_t *)(P1_BASE + p4_offset * P3_STRIDE + p3_offset * P2_STRIDE + p2_offset * P1_STRIDE);
}

uintptr_t *vmm_get_p1_entry(uintptr_t vma) {
    uintptr_t p4_offset = (vma >> 39) & 0777;
    uintptr_t p3_offset = (vma >> 30) & 0777;
    uintptr_t p2_offset = (vma >> 21) & 0777;
    uintptr_t p1_offset = (vma >> 12) & 0777;
    return (uintptr_t *)(P1_BASE + p4_offset * P3_STRIDE + p3_offset * P2_STRIDE + p2_offset * P1_STRIDE + p1_offset * SIZEOF_ENTRY);
}

#else /* NOT __vmm_use_recursive */

/*
 * TODO: THIS IS REALLY INEFFICIENT
 *
 * Each level always calls the one above it!!
 */

uintptr_t *vmm_get_p4_table(uintptr_t vma) {
    uintptr_t p4;
    asm volatile ("mov %%cr3, %0" : "=a"(p4));
    return (uintptr_t *)(p4 & PAGE_MASK_4K);
}

uintptr_t *vmm_get_p4_entry(uintptr_t vma) {
    uintptr_t p4_offset = (vma >> 39) & 0777;
    return vmm_get_p4_table(vma) + p4_offset;
}

uintptr_t *vmm_get_p3_table(uintptr_t vma) {
    return (uintptr_t *)(*vmm_get_p4_entry(vma) & PAGE_MASK_4K);
}

uintptr_t *vmm_get_p3_entry(uintptr_t vma) {
    uintptr_t p3_offset = (vma >> 30) & 0777;
    return vmm_get_p3_table(vma) + p3_offset;
}

uintptr_t *vmm_get_p2_table(uintptr_t vma) {
    return (uintptr_t *)(*vmm_get_p3_entry(vma) & PAGE_MASK_4K);
}

uintptr_t *vmm_get_p2_entry(uintptr_t vma) {
    uintptr_t p2_offset = (vma >> 21) & 0777;
    return vmm_get_p2_table(vma) + p2_offset;
}

uintptr_t *vmm_get_p1_table(uintptr_t vma) {
    return (uintptr_t *)(*vmm_get_p2_entry(vma) & PAGE_MASK_4K);
}

uintptr_t *vmm_get_p1_entry(uintptr_t vma) {
    uintptr_t p1_offset = (vma >> 12) & 0777;
    return vmm_get_p1_table(vma) + p1_offset;
}

#endif /* __vmm_use_recursive */


uintptr_t vmm_virt_to_phy(uintptr_t virtual) {
    //DEBUG_PRINTF("resolve %p\n", virtual);

    uintptr_t p4 = *vmm_get_p4_entry(virtual);
    //DEBUG_PRINTF("p4 entry is %p\n", p4);
    if (!(p4 & PAGE_PRESENT)) return -1;

    uintptr_t p3 = *vmm_get_p3_entry(virtual);
    //DEBUG_PRINTF("p3 entry is %p\n", p3);
    if (!(p3 & PAGE_PRESENT)) return -1;
    if (p3 & PAGE_ISHUGE) return (p3 & PAGE_MASK_1G) + (virtual & PAGE_OFFSET_1G);

    uintptr_t p2 = *vmm_get_p2_entry(virtual);
    //DEBUG_PRINTF("p2 entry is %p\n", p2);
    if (!(p2 & PAGE_PRESENT)) return -1;
    if (p2 & PAGE_ISHUGE) return (p2 & PAGE_MASK_2M) + (virtual & PAGE_OFFSET_2M);

    uintptr_t p1 = *vmm_get_p1_entry(virtual);
    //DEBUG_PRINTF("p1 entry is %p\n", p1);
    if (!(p1 & PAGE_PRESENT)) return -1;
    return (p1 & PAGE_MASK_4K) + (virtual & PAGE_OFFSET_4K);
}

void make_next_table(uintptr_t *table_location, uintptr_t flags) {
    uintptr_t physical = pmm_allocate_page();
    *table_location = physical | flags;
}

bool vmm_map(uintptr_t virtual, uintptr_t physical, int flags) {
    DEBUG_PRINTF("map %p to %p\n", virtual, physical);

    uintptr_t *p4_entry = vmm_get_p4_entry(virtual);
    if (!(*p4_entry & PAGE_PRESENT)) {
        DEBUG_PRINTF("Creating new p4 entry and p3 table for %p\n", virtual);

        if (virtual < 0xFFFF000000000000) {
            make_next_table(p4_entry, PAGE_WRITEABLE | PAGE_PRESENT | PAGE_USERMODE);
        } else {
            make_next_table(p4_entry, PAGE_WRITEABLE | PAGE_PRESENT);
        }

        memset(vmm_get_p3_table(virtual), 0, 0x1000);
    }

    uintptr_t *p3_entry = vmm_get_p3_entry(virtual);
    if (*p3_entry & PAGE_ISHUGE) {
        return false; // can't map inside a huge page
    }
    if (!(*p3_entry & PAGE_PRESENT)) {
        DEBUG_PRINTF("Creating new p3 entry and p2 table for %p\n", virtual);

        if (virtual < 0xFFFF000000000000) {
            make_next_table(p3_entry, PAGE_WRITEABLE | PAGE_PRESENT | PAGE_USERMODE);
        } else {
            make_next_table(p3_entry, PAGE_WRITEABLE | PAGE_PRESENT);
        }

        memset(vmm_get_p2_table(virtual), 0, 0x1000);
    }

    uintptr_t *p2_entry = vmm_get_p2_entry(virtual);
    if (*p2_entry & PAGE_ISHUGE) {
        return false; // can't map inside a huge page
    }
    if (!(*p2_entry & PAGE_PRESENT)) {
        DEBUG_PRINTF("Creating new p2 entry and p1 table for %p\n", virtual);

        if (virtual < 0xFFFF000000000000) {
            make_next_table(p2_entry, PAGE_WRITEABLE | PAGE_PRESENT | PAGE_USERMODE);
        } else {
            make_next_table(p2_entry, PAGE_WRITEABLE | PAGE_PRESENT);
        }

        memset(vmm_get_p1_table(virtual), 0, 0x1000);
    }

    uintptr_t *p1_entry = vmm_get_p1_entry(virtual);

    *p1_entry = physical | flags;
    return true;
}

// Maps contiguous virtual memory to contiguous physical memory
void vmm_map_range(uintptr_t virtual, uintptr_t physical, uintptr_t len, int flags) {
    virtual &= PAGE_MASK_4K;
    physical &= PAGE_MASK_4K;
    len /= 0x1000;
    if (len == 0)  len = 1;

    for (uintptr_t i=0; i<len; i++) {
        vmm_map(virtual + i * 0x1000, physical + i * 0x1000, flags);
    }
}

bool vmm_edit_flags(uintptr_t vma, int flags) {
    vma &= PAGE_MASK_4K;
    DEBUG_PRINTF("edit %p\n", vma);

    uintptr_t p4 = *vmm_get_p4_entry(vma);
    DEBUG_PRINTF("p4 entry is %p\n", p4);
    if (!(p4 & PAGE_PRESENT)) return false;

    uintptr_t p3 = *vmm_get_p3_entry(vma);
    DEBUG_PRINTF("p3 entry is %p\n", p3);
    if (!(p3 & PAGE_PRESENT)) return false;
    if (p3 & PAGE_ISHUGE) return (p3 & PAGE_MASK_1G) + (vma & PAGE_OFFSET_1G);

    uintptr_t p2 = *vmm_get_p2_entry(vma);
    DEBUG_PRINTF("p2 entry is %p\n", p2);
    if (!(p2 & PAGE_PRESENT)) return false;
    if (p2 & PAGE_ISHUGE) return (p2 & PAGE_MASK_2M) + (vma & PAGE_OFFSET_2M);

    uintptr_t *p1 = vmm_get_p1_entry(vma);
    DEBUG_PRINTF("p1 entry is %p\n", p1);
    if (!(*p1 & PAGE_PRESENT)) return false;
    uintptr_t tmp_p1 = (*p1 & PAGE_MASK_4K) | flags | PAGE_PRESENT;
    *p1 = tmp_p1;
    invlpg(vma);

    return true;
}

void vmm_create_unbacked(uintptr_t vma, int flags) {
    if (flags & PAGE_PRESENT) {
        panic("vmm_create_unbacked(%#lx, %x): flags cannot include PRESENT\n", vma, flags);
    }

    // I tell an unbacked existing mapping from a non-exitent one
    // by there being anything stored at the P1 entry for that page.
    // Even if there are no flags, something needs to go there:
    //
    // This is *not* one of the reserved spaces in the page mapping!
    // all bits are ignored when the PAGE_PRESENT bit is 0, so it
    // doesn't need to fit the proper mold, and I do actually need those
    // for more important things, so this is just a random bit somewhere
    // in the address.
    // The reserved bits are exposed here as PAGE_OS_RESERVED{1,2,3}
    // and are (at time of writing) only being used for 
    // PAGE_COPYONWRITE

    flags |= PAGE_UNBACKED;

    // That will be erased by the page fault routine when this is hit.

    vmm_map(vma, 0, flags);
}

int vmm_do_page_fault(uintptr_t fault_addr) {
    // debug: printf("vmm_do_page_fault\n");

    uintptr_t p4 = *vmm_get_p4_entry(fault_addr);
    if (!(p4 & PAGE_PRESENT)) return 0;

    uintptr_t p3 = *vmm_get_p3_entry(fault_addr);
    if (!(p3 & PAGE_PRESENT)) return 0;

    uintptr_t p2 = *vmm_get_p2_entry(fault_addr);
    if (!(p2 & PAGE_PRESENT)) return 0;

    uintptr_t *p1 = vmm_get_p1_entry(fault_addr);

    if (*p1 & PAGE_UNBACKED && !(*p1 & PAGE_PRESENT)) {
        // if the page structure exists and the page is marked unbacked
        
        // debug: printf("vmm: backing unbacked memory at %lx\n", fault_addr);
        uintptr_t phy = pmm_allocate_page();

        *p1 &= PAGE_FLAGS_MASK; // remove any extra bits (see create_unbacked)
        *p1 |= phy | PAGE_PRESENT;

        return 1;
    } else if (*p1 & PAGE_COPYONWRITE) {
        //printf("vmm: copying COW page at %lx\n", fault_addr);

        void *temp_page = malloc(0x1000);
        memcpy(temp_page, (void *)(fault_addr & PAGE_MASK_4K), 0x1000);

        uintptr_t phy = pmm_allocate_page();

        *p1 &= (PAGE_FLAGS_MASK & ~PAGE_COPYONWRITE);
        *p1 |= phy | PAGE_PRESENT | PAGE_WRITEABLE;

        memcpy((void *)(fault_addr & PAGE_MASK_4K), temp_page, 0x1000);

        return 1;
    } else {
        return 0;
    }
}

#define FORK_ENTRY (uintptr_t)0401

#define FORK_P1_BASE (0xFFFF000000000000 + (FORK_ENTRY << 39))
#define FORK_P2_BASE (FORK_P1_BASE + (FORK_ENTRY << 30))
#define FORK_P3_BASE (FORK_P2_BASE + (FORK_ENTRY << 21))
#define FORK_P4_BASE (FORK_P3_BASE + (FORK_ENTRY << 12))

uintptr_t *vmm_get_p4_table_fork(uintptr_t vma) {
    return (uintptr_t *)FORK_P4_BASE;
}

uintptr_t *vmm_get_p4_entry_fork(uintptr_t vma) {
    uintptr_t p4_offset = (vma >> 39) & 0777;
    return (uintptr_t *)(FORK_P4_BASE + p4_offset * SIZEOF_ENTRY);
}

uintptr_t *vmm_get_p3_table_fork(uintptr_t vma) {
    uintptr_t p4_offset = (vma >> 39) & 0777;
    return (uintptr_t *)(FORK_P3_BASE + p4_offset * P1_STRIDE);
}

uintptr_t *vmm_get_p3_entry_fork(uintptr_t vma) {
    uintptr_t p4_offset = (vma >> 39) & 0777;
    uintptr_t p3_offset = (vma >> 30) & 0777;
    return (uintptr_t *)(FORK_P3_BASE + p4_offset * P1_STRIDE + p3_offset * SIZEOF_ENTRY);
}

uintptr_t *vmm_get_p2_table_fork(uintptr_t vma) {
    uintptr_t p4_offset = (vma >> 39) & 0777;
    uintptr_t p3_offset = (vma >> 30) & 0777;
    return (uintptr_t *)(FORK_P2_BASE + p4_offset * P2_STRIDE + p3_offset * P1_STRIDE);
}

uintptr_t *vmm_get_p2_entry_fork(uintptr_t vma) {
    uintptr_t p4_offset = (vma >> 39) & 0777;
    uintptr_t p3_offset = (vma >> 30) & 0777;
    uintptr_t p2_offset = (vma >> 21) & 0777;
    return (uintptr_t *)(FORK_P2_BASE + p4_offset * P2_STRIDE + p3_offset * P1_STRIDE + p2_offset * SIZEOF_ENTRY);
}

uintptr_t *vmm_get_p1_table_fork(uintptr_t vma) {
    uintptr_t p4_offset = (vma >> 39) & 0777;
    uintptr_t p3_offset = (vma >> 30) & 0777;
    uintptr_t p2_offset = (vma >> 21) & 0777;
    return (uintptr_t *)(FORK_P1_BASE + p4_offset * P3_STRIDE + p3_offset * P2_STRIDE + p2_offset * P1_STRIDE);
}

uintptr_t *vmm_get_p1_entry_fork(uintptr_t vma) {
    uintptr_t p4_offset = (vma >> 39) & 0777;
    uintptr_t p3_offset = (vma >> 30) & 0777;
    uintptr_t p2_offset = (vma >> 21) & 0777;
    uintptr_t p1_offset = (vma >> 12) & 0777;
    return (uintptr_t *)(FORK_P1_BASE + p4_offset * P3_STRIDE + p3_offset * P2_STRIDE + p2_offset * P1_STRIDE + p1_offset * SIZEOF_ENTRY);
}

int copy_p1(size_t p4ix, size_t p3ix, size_t p2ix) {
    uintptr_t *cur_p1 = (void *)P1_BASE + p4ix * P3_STRIDE + p3ix * P2_STRIDE + p2ix * P1_STRIDE;
    uintptr_t *fork_p1 = (void *)FORK_P1_BASE + p4ix * P3_STRIDE + p3ix * P2_STRIDE + p2ix * P1_STRIDE;

    if (p4ix == 510) {
        uintptr_t new_stack = pmm_allocate_page() | PAGE_WRITEABLE | PAGE_PRESENT;

        fork_p1[0] = new_stack;

        return 0;
    }

    for (size_t i=0; i<512; i++) {
        if (cur_p1[i]) {
            fork_p1[i] = cur_p1[i]; // point to same memory with COW

            if (fork_p1[i] & PAGE_WRITEABLE) {
                fork_p1[i] &= ~PAGE_WRITEABLE;
                fork_p1[i] |= PAGE_COPYONWRITE;
            }
            if (cur_p1[i] & PAGE_WRITEABLE) {
                cur_p1[i] &= ~PAGE_WRITEABLE;
                cur_p1[i] |= PAGE_COPYONWRITE;
            }
        }
    }
    return 0;
}

int copy_p2(size_t p4ix, size_t p3ix) {
    uintptr_t *cur_p2 = (void *)P2_BASE + p4ix * P2_STRIDE + p3ix * P1_STRIDE;
    uintptr_t *fork_p2 = (void *)FORK_P2_BASE + p4ix * P2_STRIDE + p3ix * P1_STRIDE;

    for (size_t i=0; i<512; i++) {
        if (cur_p2[i]) {
            fork_p2[i] = cur_p2[i] & PAGE_FLAGS_MASK;
            fork_p2[i] |= pmm_allocate_page();
            
            copy_p1(p4ix, p3ix, i);
        }
    }
    return 0;
}

int copy_p3(size_t p4ix) {
    uintptr_t *cur_p3 = (void *)P3_BASE + p4ix * P1_STRIDE;
    uintptr_t *fork_p3 = (void *)FORK_P3_BASE + p4ix * P1_STRIDE;

    for (size_t i=0; i<512; i++) {

        if (cur_p3[i]) {
            fork_p3[i] = cur_p3[i] & PAGE_FLAGS_MASK;
            fork_p3[i] |= pmm_allocate_page();
            
            copy_p2(p4ix, i);
        }
    }
    return 0;
}

int copy_p4() {
    uintptr_t *cur_pml4 = (void *)P4_BASE;
    uintptr_t *fork_pml4 = (void *)FORK_P4_BASE;

    size_t i;

    for (i=0; i<256; i++) {
        if (cur_pml4[i]) {
            fork_pml4[i] = cur_pml4[i] & PAGE_FLAGS_MASK;
            fork_pml4[i] |= pmm_allocate_page();
            copy_p3(i);
        }
    }

    i = 510; // unique int stacks per process vm

    fork_pml4[i] = cur_pml4[i] & PAGE_FLAGS_MASK;
    fork_pml4[i] |= pmm_allocate_page();
    copy_p3(i);

    return 0;
}

int vmm_fork() {
    uintptr_t fork_pml4_phy = pmm_allocate_page();
    DEBUG_PRINTF("vmm: new recursive map at phy:%lx\n", fork_pml4_phy);

    vmm_map(FORK_P4_BASE, fork_pml4_phy, PAGE_WRITEABLE | PAGE_PRESENT);
    memset((void *)FORK_P4_BASE, 0, 0x1000);

    uintptr_t *cur_pml4 = (void *)P4_BASE;
    uintptr_t *fork_pml4 = (void *)FORK_P4_BASE;

    fork_pml4[511] = cur_pml4[511];
    cur_pml4[257] = fork_pml4_phy | PAGE_PRESENT | PAGE_WRITEABLE; // just for this part
    fork_pml4[257] = fork_pml4_phy | PAGE_PRESENT | PAGE_WRITEABLE; // just for this part

    // PML4 258-510 are reserved and not copied at this time.

    copy_p4();

    /*
     * A good idea, but a flawed implementation - I need a seperate PDPT, PD, and PT for this
     *
     * I could use PML4:510 or perhaps (existing)PDPT:511 or something

    extern uintptr_t int_stack; // different interrupt stacks per process vm
    uintptr_t int_stack_addr = (uintptr_t)&int_stack;
    uintptr_t *stack_entry = vmm_get_p1_entry_fork(int_stack_addr);
    *stack_entry = pmm_allocate_page() | PAGE_PRESENT | PAGE_WRITEABLE;
    */

    fork_pml4[257] = 0;
    fork_pml4[256] = fork_pml4_phy | PAGE_PRESENT | PAGE_WRITEABLE; // actual recursive map
    cur_pml4[257] = 0;

    // reset TLB
    uintptr_t cr3;
    asm volatile ("mov %%cr3, %0" : "=a"(cr3));
    asm volatile ("mov %0, %%cr3" :: "a"(cr3));

    return fork_pml4_phy;
}

