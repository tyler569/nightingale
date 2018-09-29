
#include <basic.h>
#include <string.h>
#include <panic.h>
#include <malloc.h>
// #define DEBUG
#include <debug.h>
#include <pmm.h>
#include "cpu.h"
#include "vmm.h"

#define REC_ENTRY (uintptr_t)1023

#define PT_BASE (REC_ENTRY << 22)
#define PD_BASE (PT_BASE + (REC_ENTRY << 12))

#define SIZEOF_ENTRY sizeof(uintptr_t)
#define PT_STRIDE PAGE_SIZE / SIZEOF_ENTRY

uintptr_t* vmm_get_pd_table(uintptr_t vma) {
    return (uintptr_t*)PD_BASE;
}

uintptr_t* vmm_get_pd_entry(uintptr_t vma) {
    uintptr_t pd_offset = (vma >> 22) & 01777;
    return (uintptr_t*)PD_BASE + pd_offset;
}

uintptr_t* vmm_get_pt_table(uintptr_t vma) {
    uintptr_t pd_offset = (vma >> 22) & 01777;
    return (uintptr_t*)PT_BASE + pd_offset * PT_STRIDE;
}

uintptr_t* vmm_get_pt_entry(uintptr_t vma) {
    uintptr_t pd_offset = (vma >> 22) & 01777;
    uintptr_t pt_offset = (vma >> 12) & 01777;
    return (uintptr_t*)PT_BASE + pd_offset * PT_STRIDE + pt_offset;
}

uintptr_t vmm_virt_to_phy(uintptr_t virtual) {
    uintptr_t pd = *vmm_get_pd_entry(virtual);
    if (!(pd & PAGE_PRESENT)) return -1;
    if (pd & PAGE_ISHUGE) return (pd & PAGE_MASK_4M) + (virtual & PAGE_OFFSET_4M);

    uintptr_t pt = *vmm_get_pt_entry(virtual);
    if (!(pt & PAGE_PRESENT)) return -1;
    return (pt & PAGE_MASK_4K) + (virtual & PAGE_OFFSET_4K);
}

uintptr_t vmm_resolve(uintptr_t virtual) {
    uintptr_t pd = *vmm_get_pd_entry(virtual);
    if (!(pd & PAGE_PRESENT)) return -1;
    if (pd & PAGE_ISHUGE) return pd;

    uintptr_t pt = *vmm_get_pt_entry(virtual);
    if (!(pt & PAGE_PRESENT)) return -1;
    return pt;
}

void make_next_table(uintptr_t* table_location, uintptr_t flags) {
    uintptr_t physical = pmm_allocate_page();
    *table_location = physical | flags;
}

// TODO: move this (in vmm64 too)
#define KERNEL_BASE 0x80000000

bool vmm_map(uintptr_t virtual, uintptr_t physical, int flags) {
    // DEBUG_PRINTF("map %p to %p\n", virtual, physical);

    uintptr_t* pd_entry = vmm_get_pd_entry(virtual);
    if (!(*pd_entry & PAGE_PRESENT)) {
        DEBUG_PRINTF("Creating new pd entry and pt table for %p\n", virtual);

        if (virtual < KERNEL_BASE) {
            make_next_table(pd_entry, PAGE_WRITEABLE | PAGE_PRESENT | PAGE_USERMODE);
        } else {
            make_next_table(pd_entry, PAGE_WRITEABLE | PAGE_PRESENT);
        }

        memset(vmm_get_pt_table(virtual), 0, PAGE_SIZE);
    }

    uintptr_t* pt_entry = vmm_get_pt_entry(virtual);

    *pt_entry = physical | flags;
    return true;
}

bool vmm_unmap(uintptr_t virtual) {
    DEBUG_PRINTF("unmap %p\n", virtual);

    uintptr_t* pd_entry = vmm_get_pd_entry(virtual);
    if (!(*pd_entry & PAGE_PRESENT)) {
        return true;
    }

    uintptr_t* pt_entry = vmm_get_pt_entry(virtual);

    *pt_entry = 0;
    return true;
}

// Maps contiguous virtual memory to contiguous physical memory
void vmm_map_range(uintptr_t virtual, uintptr_t physical, uintptr_t len, int flags) {
    virtual &= PAGE_MASK_4K;
    physical &= PAGE_MASK_4K;
    len /= PAGE_SIZE;
    if (len == 0)  len = 1;

    for (uintptr_t i=0; i<len; i++) {
        vmm_map(virtual + i * PAGE_SIZE, physical + i * PAGE_SIZE, flags);
    }
}

bool vmm_edit_flags(uintptr_t vma, int flags) {
    vma &= PAGE_MASK_4K;

    uintptr_t pd = *vmm_get_pd_entry(vma);
    if (!(pd & PAGE_PRESENT)) return false;

    uintptr_t* pt = vmm_get_pt_entry(vma);
    if (!(*pt & PAGE_PRESENT)) return false;

    uintptr_t tmp_pt = (*pt & PAGE_MASK_4K) | flags | PAGE_PRESENT;
    *pt = tmp_pt;
    invlpg(vma);

    return true;
}

void vmm_create_unbacked(uintptr_t vma, int flags) {
    // DEBUG_PRINTF("vmm_create_unbacked(%p, %x)\n", vma, flags);
    if (flags & PAGE_PRESENT) {
        panic("vmm_create_unbacked(%#zx, %x): flags cannot include PRESENT\n", vma, flags);
    }

    if (vmm_virt_to_phy(vma) != -1) {
        // creating a page that exists is a noop
        return;
    }

    // I tell an unbacked existing mapping from a non-exitent one
    // by there being anything stored at the pt entry for that page.
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

void vmm_create_unbacked_range(uintptr_t vma, size_t len, int flags) {
    for (uintptr_t page = vma; page <= vma + len; page += PAGE_SIZE) {
        vmm_create_unbacked(page, flags);
    }
}

// lock this?
char temp_page[PAGE_SIZE];

int vmm_do_page_fault(uintptr_t fault_addr) {
    // DEBUG_PRINTF("vmm_do_page_fault(%p)\n", fault_addr);

    uintptr_t pd = *vmm_get_pd_entry(fault_addr);
    if (!(pd & PAGE_PRESENT)) return 0;

    uintptr_t* ppt = vmm_get_pt_entry(fault_addr);
    uintptr_t pt = *ppt;

    if (pt & PAGE_UNBACKED && !(pt & PAGE_PRESENT)) {
        // if the page structure exists and the page is marked unbacked
        
        // debug: 
        // DEBUG_PRINTF("vmm: backing unbacked memory at %zx\n", fault_addr);
        uintptr_t phy = pmm_allocate_page();

        *ppt &= PAGE_FLAGS_MASK; // remove any extra bits (see create_unbacked)
        *ppt |= phy | PAGE_PRESENT;

        return 1;
    } else if (pt & PAGE_COPYONWRITE) {
        //printf("vmm: copying COW page at %zx\n", fault_addr);

        // void*temp_page = malloc(0x1000);
        memcpy(temp_page, (char*)(fault_addr & PAGE_MASK_4K), PAGE_SIZE);

        uintptr_t phy = pmm_allocate_page();

        *ppt &= (PAGE_FLAGS_MASK & ~PAGE_COPYONWRITE);
        *ppt |= phy | PAGE_PRESENT | PAGE_WRITEABLE;

        memcpy((char*)(fault_addr & PAGE_MASK_4K), temp_page, PAGE_SIZE);
        invlpg(fault_addr);

        return 1;
    } else {
        return 0;
    }
}

#define FORK_ENTRY (uintptr_t)1022

#define FORK_PT_BASE (FORK_ENTRY << 22)
#define FORK_PD_BASE (FORK_PT_BASE + (FORK_ENTRY << 12))

uintptr_t* vmm_get_pd_table_fork(uintptr_t vma) {
    return (uintptr_t*)FORK_PD_BASE;
}

uintptr_t* vmm_get_pd_entry_fork(uintptr_t vma) {
    uintptr_t pd_offset = (vma >> 22) & 01777;
    return (uintptr_t*)FORK_PD_BASE + pd_offset;
}

uintptr_t* vmm_get_pt_table_fork(uintptr_t vma) {
    uintptr_t pd_offset = (vma >> 22) & 01777;
    return (uintptr_t*)FORK_PT_BASE + pd_offset * PT_STRIDE;
}

uintptr_t* vmm_get_pt_entry_fork(uintptr_t vma) {
    uintptr_t pd_offset = (vma >> 22) & 01777;
    uintptr_t pt_offset = (vma >> 12) & 01777;
    return (uintptr_t*)FORK_PT_BASE + pd_offset * PT_STRIDE + pt_offset * SIZEOF_ENTRY;
}

int copy_pt(size_t pdix) {
    uintptr_t* cur_pt = (uintptr_t*)PT_BASE + pdix * PT_STRIDE;
    uintptr_t* fork_pt = (uintptr_t*)FORK_PT_BASE + pdix * PT_STRIDE;

    for (size_t i=0; i<1024; i++) {
        if (cur_pt[i]) {
            fork_pt[i] = cur_pt[i]; // point to same memory with COW

            if (!(fork_pt[i] & PAGE_PRESENT)) {
                // is unbacked, no need to change
            } else {
                if (fork_pt[i] & PAGE_WRITEABLE) {
                    fork_pt[i] &= ~PAGE_WRITEABLE;
                    fork_pt[i] |= PAGE_COPYONWRITE;
                }
                if (cur_pt[i] & PAGE_WRITEABLE) {
                    cur_pt[i] &= ~PAGE_WRITEABLE;
                    cur_pt[i] |= PAGE_COPYONWRITE;
                }
            }
        }
    }
    return 0;
}

int copy_pd() {
    uintptr_t* cur_pd = (uintptr_t*)PD_BASE;
    uintptr_t* fork_pd = (uintptr_t*)FORK_PD_BASE;

    for (size_t i=0; i<512; i++) {
        if (cur_pd[i]) {
            fork_pd[i] = cur_pd[i] & PAGE_FLAGS_MASK;
            fork_pd[i] |= pmm_allocate_page();
            copy_pt(i);
        }
    }
    return 0;
}

int vmm_fork() {
    disable_irqs(); // this definitely can't be interrupted by a task switch

    uintptr_t fork_pd_phy = pmm_allocate_page();
    DEBUG_PRINTF("vmm: new recursive map at phy:%zx\n", fork_pd_phy);

    vmm_map(FORK_PD_BASE, fork_pd_phy, PAGE_WRITEABLE | PAGE_PRESENT);
    memset((void*)FORK_PD_BASE, 0, PAGE_SIZE);

    uintptr_t* cur_pd = (uintptr_t*)PD_BASE;
    uintptr_t* fork_pd = (uintptr_t*)FORK_PD_BASE;

    for (int i=512; i<1022; i++) {
        fork_pd[i] = cur_pd[i]; // global kernel pages
    }
    cur_pd[1022] = fork_pd_phy | PAGE_PRESENT | PAGE_WRITEABLE; // just for this part
    fork_pd[1022] = fork_pd_phy | PAGE_PRESENT | PAGE_WRITEABLE; // just for this part

    copy_pd();

    fork_pd[1022] = 0;
    fork_pd[1023] = fork_pd_phy | PAGE_PRESENT | PAGE_WRITEABLE; // actual recursive map
    cur_pd[1022] = 0;

    // reset TLB
    uintptr_t cr3;
    asm volatile ("mov %%cr3, %0" : "=a"(cr3));
    asm volatile ("mov %0, %%cr3" :: "a"(cr3));

    enable_irqs();
    return fork_pd_phy;
}

void vmm_early_init(void) {
    *vmm_get_pd_entry(0) = 0;

    // TODO:
    // extern char hhstack_guard_page;
    // *vmm_get_pt_entry((uintptr_t)&hhstack_guard_page) = 0;
}

