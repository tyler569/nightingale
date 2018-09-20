
#pragma once
#ifndef NIGHTINGALE_PAGING_H
#define NIGHTINGALE_PAGING_H

#include <basic.h>

/*
 * Mull on this as a potential API for dealing with
 * vmm table entries - it's pretty magic, but it's
 * simple enough and does seem to work.
 */
typedef union PageEntry {
    struct {
        bool present : 1;
        bool writeable : 1;
        bool usermode : 1;
        bool writethrough : 1;
        bool cachedisable : 1;
        bool accessed : 1;
        bool dirty : 1;
        bool ishuge : 1;
        uintptr_t ignored : 3;
        bool pat : 1;
        uintptr_t address : 51;
    } __attribute__((packed));

    uintptr_t value;
} PageEntry;

#define PAGE_PRESENT 0x01
#define PAGE_WRITEABLE 0x02
#define PAGE_USERMODE 0x04
#define PAGE_ACCESSED 0x20
#define PAGE_DIRTY 0x40
#define PAGE_ISHUGE 0x80
#define PAGE_GLOBAL 0x100

#define PAGE_OS_RESERVED1 0x200
#define PAGE_OS_RESERVED2 0x400
#define PAGE_OS_RESERVED3 0x800

#define PAGE_COPYONWRITE PAGE_OS_RESERVED1

#define PAGE_UNBACKED 0x100000

#define PAGE_OFFSET_1G 07777777777 // (3 + 3 + 4) * 3 = 30
#define PAGE_OFFSET_2M    07777777 // (3 + 4)     * 3 = 21
#define PAGE_OFFSET_4K       07777 // 4           * 3 = 12

#define PAGE_MASK_1G (~PAGE_OFFSET_1G)
#define PAGE_MASK_2M (~PAGE_OFFSET_2M)
#define PAGE_MASK_4K (~PAGE_OFFSET_4K)

#define PAGE_FLAGS_MASK 0xFF00000000000FFF
#define PAGE_ADDR_MASK  0x00FFFFFFFFFFF000

uintptr_t *vmm_get_p4_table(uintptr_t vma);
uintptr_t *vmm_get_p4_entry(uintptr_t vma);
uintptr_t *vmm_get_p3_table(uintptr_t vma);
uintptr_t *vmm_get_p3_entry(uintptr_t vma);
uintptr_t *vmm_get_p2_table(uintptr_t vma);
uintptr_t *vmm_get_p2_entry(uintptr_t vma);
uintptr_t *vmm_get_p1_table(uintptr_t vma);
uintptr_t *vmm_get_p1_entry(uintptr_t vma);

uintptr_t *vmm_get_p4_table_fork(uintptr_t vma);
uintptr_t *vmm_get_p4_entry_fork(uintptr_t vma);
uintptr_t *vmm_get_p3_table_fork(uintptr_t vma);
uintptr_t *vmm_get_p3_entry_fork(uintptr_t vma);
uintptr_t *vmm_get_p2_table_fork(uintptr_t vma);
uintptr_t *vmm_get_p2_entry_fork(uintptr_t vma);
uintptr_t *vmm_get_p1_table_fork(uintptr_t vma);
uintptr_t *vmm_get_p1_entry_fork(uintptr_t vma);

uintptr_t vmm_virt_to_phy(uintptr_t vma);
uintptr_t vmm_resolve(uintptr_t vma);
bool vmm_map(uintptr_t vma, uintptr_t pma, int flags);
void vmm_map_range(uintptr_t vma, uintptr_t pma, size_t len, int flags);
bool vmm_edit_flags(uintptr_t vma, int flags);

void vmm_create_unbacked(uintptr_t vma, int flags);
void vmm_create_unbacked_range(uintptr_t vma, size_t len, int flags);
//void vmm_create_at_range(uintptr_t base, size_t len, int flags);

int vmm_fork();

#endif

