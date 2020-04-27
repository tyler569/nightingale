
#pragma once
#ifndef NG_X86_64_VMM_H
#define NG_X86_64_VMM_H

#include <basic.h>
#include <ng/types.h>

#define VMM_VIRTUAL_OFFSET 0xFFFFFFFF80000000

#define PAGE_PRESENT 0x01
#define PAGE_WRITEABLE 0x02
#define PAGE_USERMODE 0x04
#define PAGE_ACCESSED 0x20
#define PAGE_DIRTY 0x40
#define PAGE_ISHUGE 0x80
#define PAGE_GLOBAL 0x100

#define PAGE_SIZE 0x1000

#define PAGE_OS_RESERVED1 0x200
#define PAGE_OS_RESERVED2 0x400
#define PAGE_OS_RESERVED3 0x800

#define PAGE_COPYONWRITE PAGE_OS_RESERVED1
#define PAGE_STACK_GUARD PAGE_OS_RESERVED2

#define PAGE_UNBACKED 0x100000

#define PAGE_OFFSET_1G 07777777777UL // (3 + 3 + 4) * 3 = 30
#define PAGE_OFFSET_2M    07777777UL //     (3 + 4) * 3 = 21
#define PAGE_OFFSET_4K       07777UL //          4  * 3 = 12

#define PAGE_MASK_1G (~PAGE_OFFSET_1G)
#define PAGE_MASK_2M (~PAGE_OFFSET_2M)
#define PAGE_MASK_4K (~PAGE_OFFSET_4K)

#define PAGE_FLAGS_MASK 0xFF00000000000FFFUL
#define PAGE_ADDR_MASK 0x00FFFFFFFFFFF000UL


typedef uintptr_t pte_t;

void reset_tlb();
bool vmm_check(virt_addr_t vma);

pte_t vmm_pte(virt_addr_t vma);
pte_t vmm_fork_pte(virt_addr_t vma);

phys_addr_t vmm_phy(virt_addr_t vma);
phys_addr_t vmm_fork_phy(virt_addr_t vma);

int vmm_map(virt_addr_t vma, phys_addr_t pma, int flags);
int vmm_fork_map(virt_addr_t vma, phys_addr_t pma, int flags);

int vmm_unmap(virt_addr_t vma);
int vmm_fork_unmap(virt_addr_t vma);

void vmm_map_range(virt_addr_t vma, phys_addr_t pma, size_t len, int flags);
void vmm_fork_map_range(virt_addr_t vma, phys_addr_t pma, size_t len, int flags);

void vmm_create_unbacked(virt_addr_t vma, int flags);
void vmm_fork_create_unbacked(virt_addr_t vma, int flags);
void vmm_create_unbacked_range(virt_addr_t vma, size_t len, int flags);
void vmm_fork_create_unbacked_range(virt_addr_t vma, size_t len, int flags);

void vmm_fork_copyfrom(virt_addr_t fork_base, virt_addr_t this_base, int pages);
void vmm_remap(virt_addr_t base, virt_addr_t top, int vm_flags);

void vmm_set_fork_base(phys_addr_t fork_p4_phy);
void vmm_clear_fork_base();
void vmm_set_pgtable(phys_addr_t pgroot);

int vmm_do_page_fault(virt_addr_t fault_addr);

#endif // NG_X86_64_VMM_H

