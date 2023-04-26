#include <basic.h>
#include <ng/pmm.h>
#include <string.h>
#include <x86/vmm.h>
#include "vmm2.h"

static size_t table_index(virt_addr_t addr, int table)
{
    assert(table > 0 && table < 5);

    return addr >> 12 >> ((table - 1) * 9) & 0x1FF;
}

#define PHYS_MAP 0xFF80000000000000
#define PTR(phys_addr) (void *)(PHYS_MAP + (phys_addr))

static void make_table(uint64_t *ptr, bool kernel)
{
    phys_addr_t next_page = pm_alloc();
    char *next_page_ptr = PTR(next_page);
    memset(next_page_ptr, 0, PAGE_SIZE);

    *ptr = next_page | PAGE_PRESENT | PAGE_WRITEABLE
        | (kernel ? PAGE_USERMODE : 0);
}

static uint64_t *vmm2_pte(phys_addr_t root, virt_addr_t addr)
{
    uint64_t *pt = PTR(root);
    uint64_t *pte = NULL;
    for (int table = 4; table > 0; table--) {
        size_t index = table_index(addr, table);
        pte = &pt[index];
        if (!(*pte & PAGE_PRESENT) && table > 1) {
            make_table(pte, target >= VMM_KERNEL_BASE);
        }
        pt = PTR(*pte & PAGE_ADDR_MASK);
    }
    return pte;
}

void vmm2_map(phys_addr_t root, phys_addr_t target, virt_addr_t addr, int flags)
{
    *vmm2_pte(root, addr) = target | PAGE_PRESENT | flags;
}

void vmm2_unmap(phys_addr_t root, virt_addr_t addr)
{
    *vmm2_pte(root, addr) = 0;
}

void vmm2_create_unbacked(phys_addr_t root, virt_addr_t addr)
{
    *vmm2_pte(root, addr) = PAGE_UNBACKED;
}