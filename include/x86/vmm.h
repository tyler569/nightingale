#pragma once
#ifndef _X86_VMM_H_
#define _X86_VMM_H_

#include <basic.h>
#include <sys/types.h>

#define VMM_KERNEL_BASE 0xFFFFFFFF80000000

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

#define PAGE_OFFSET_1G 07777777777 // (3 + 3 + 4) * 3 = 30
#define PAGE_OFFSET_2M 07777777    // (3 + 4)     * 3 = 21
#define PAGE_OFFSET_4K 07777       // 4           * 3 = 12

#define PAGE_MASK_1G (~PAGE_OFFSET_1G)
#define PAGE_MASK_2M (~PAGE_OFFSET_2M)
#define PAGE_MASK_4K (~PAGE_OFFSET_4K)

#define PAGE_FLAGS_MASK 0xFF00000000000FFF
#define PAGE_ADDR_MASK 0x00FFFFFFFFFFF000

enum x86_fault {
    F_PRESENT = 0x01,
    F_WRITE = 0x02,
    F_USERMODE = 0x04,
    F_RESERVED = 0x08,
    F_IFETCH = 0x10,
};

enum vmm_copy_op {
    COPY_COW,
    COPY_SHARED,
    COPY_EAGER,
};

struct process;

phys_addr_t vmm_resolve(virt_addr_t vma);
phys_addr_t vmm_virt_to_phy(virt_addr_t vma);
uintptr_t *vmm_pte_ptr(virt_addr_t vma);

bool vmm_map(virt_addr_t vma, phys_addr_t pma, int flags);
void vmm_map_range(virt_addr_t vma, phys_addr_t pma, size_t len, int flags);
void vmm_create_unbacked(virt_addr_t vma, int flags);
void vmm_create_unbacked_range(virt_addr_t vma, size_t len, int flags);
bool vmm_unmap(virt_addr_t vma);
void vmm_unmap_range(virt_addr_t vma, size_t len);

phys_addr_t vmm_fork(struct process *);

void vmm_destroy_tree(phys_addr_t root);
void vmm_early_init(void);
enum fault_result vmm_do_page_fault(virt_addr_t fault_addr,
                                    enum x86_fault reason);

#endif // _X86_VMM_H_
