#include <basic.h>
#include <assert.h>
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
            make_table(pte, addr >= VMM_KERNEL_BASE);
        }
        pt = PTR(*pte & PAGE_ADDR_MASK);
    }
    return pte;
}

void vmm2_map(phys_addr_t root, phys_addr_t target, virt_addr_t addr, int flags)
{
    *vmm2_pte(root, addr) = target | PAGE_PRESENT | flags;
}

void vmm2_map_range(phys_addr_t root, phys_addr_t target, virt_addr_t addr,
    size_t size, int flags)
{
    for (size_t i = 0; i < size; i += PAGE_SIZE) {
        vmm2_map(root, target + i, addr + i, flags);
    }
}

void vmm2_unmap(phys_addr_t root, virt_addr_t addr)
{
    *vmm2_pte(root, addr) = 0;
}

void vmm2_create_unbacked(phys_addr_t root, virt_addr_t addr, int flags)
{
    *vmm2_pte(root, addr) = flags | PAGE_UNBACKED;
}

static void reset_tlb()
{
    __asm__ volatile("mov %%cr3, %%rax; mov %%rax, %%cr3" : : : "rax");
}

static void vmm2_clone_table(phys_addr_t root, phys_addr_t new_root, int level)
{
    uint64_t *table = PTR(root);
    uint64_t *new_table = PTR(new_root);
    for (int i = 0; i < 512; i++) {
        uint64_t pte = table[i];
        if (pte & PAGE_PRESENT) {
            if (level > 1) {
                phys_addr_t new_table_addr = pm_alloc();
                new_table[i] = new_table_addr | (pte & 0xFFF);
                vmm2_clone_table(
                    pte & PAGE_ADDR_MASK, new_table_addr, level - 1);
            } else {
                if (pte & PAGE_UNBACKED || !(pte & PAGE_WRITEABLE)) {
                    new_table[i] = pte;
                } else if (pte & PAGE_COPYONWRITE) {
                    table[i] = pte & ~PAGE_WRITEABLE;
                    new_table[i] = pte & ~PAGE_WRITEABLE;
                } else {
                    phys_addr_t new_page = pm_alloc();
                    memcpy(PTR(new_page), PTR(pte & PAGE_ADDR_MASK), PAGE_SIZE);
                    new_table[i] = new_page | (pte & 0xFFF);
                }
            }
        }
    }
}

void vmm2_clone(phys_addr_t root, phys_addr_t new_root)
{
    vmm2_clone_table(root, new_root, 4);
}

void vmm2_destroy_table(phys_addr_t root, int level)
{
    uint64_t *table = PTR(root);
    for (int i = 0; i < 512; i++) {
        uint64_t pte = table[i];
        phys_addr_t page = pte & PAGE_ADDR_MASK;
        if (pte & PAGE_PRESENT) {
            if (level > 1) {
                vmm2_destroy_table(page, level - 1);
                pm_free(page);
            } else {
                if (!(pte & PAGE_UNBACKED)) {
                    pm_free(page);
                }
            }
        }
    }
}

void vmm2_destroy(phys_addr_t root) { vmm2_destroy_table(root, 4); }

phys_addr_t vmm2_virt_to_phy(virt_addr_t addr)
{
    return *vmm2_pte(0, addr) & PAGE_ADDR_MASK;
}