
// #define DEBUG
#include <basic.h>
#include <ng/debug.h>
#include <ng/panic.h>
#include <ng/pmm.h>
#include <ng/vmm.h>
#include <ng/x86/64/cpu.h>
#include <nc/stdio.h>
#include <nc/string.h>

//
// More consistent naming for the page tables
//
// P4 = PML4
// P3 = PDPT
// P2 = PD
// P1 = PT
//

#define assert_aligned(v) assert(v & PAGE_OFFSET_4K == 0);

#define REC_ENTRY (uintptr_t)0400 // higher half + 0

#define P1_BASE (0xFFFF000000000000 + (REC_ENTRY << 39))
#define P2_BASE (P1_BASE + (REC_ENTRY << 30))
#define P3_BASE (P2_BASE + (REC_ENTRY << 21))
#define P4_BASE (P3_BASE + (REC_ENTRY << 12))

#define FORK_ENTRY (uintptr_t)0401

#define FORK_P1_BASE (0xFFFF000000000000 + (FORK_ENTRY << 39))
#define FORK_P2_BASE (FORK_P1_BASE + (FORK_ENTRY << 30))
#define FORK_P3_BASE (FORK_P2_BASE + (FORK_ENTRY << 21))
#define FORK_P4_BASE (FORK_P3_BASE + (FORK_ENTRY << 12))

typedef uintptr_t pte_t;

typedef int result_t;
#define R_OK            0
#define R_ERROR         1
/* etc */

struct ptes {
        pte_t *p4e, *p3e, *p2e, *p1e;
};

#define SIZEOF_ENTRY sizeof(uintptr_t)
#define P1_STRIDE 0x1000 / SIZEOF_ENTRY
#define P2_STRIDE 0x200000 / SIZEOF_ENTRY
#define P3_STRIDE 0x40000000 / SIZEOF_ENTRY

#define VM_NULL (virt_addr_t)(-1)

void reset_tlb() {
        uintptr_t cr3;
        asm volatile("mov %%cr3, %0" : "=a"(cr3));
        asm volatile("mov %0, %%cr3" ::"a"(cr3));
}

struct ptes vmm_ptes(virt_addr_t vma) {
        size_t p4_offset = (vma >> 39) & 0777;
        size_t p3_offset = (vma >> 30) & 0777;
        size_t p2_offset = (vma >> 21) & 0777;
        size_t p1_offset = (vma >> 12) & 0777;

        pte_t *p4e = (pte_t *)P4_BASE + p4_offset;
        pte_t *p3e = (pte_t *)P3_BASE + p4_offset * P1_STRIDE + p3_offset;
        pte_t *p2e = (pte_t *)P2_BASE + p4_offset * P2_STRIDE +
                    p3_offset * P1_STRIDE + p2_offset;
        pte_t *p1e = (pte_t *)P1_BASE + p4_offset * P3_STRIDE +
               p3_offset * P2_STRIDE + p2_offset * P1_STRIDE + p1_offset;

        return (struct ptes){ p4e, p3e, p2e, p1e };
}

struct ptes vmm_fork_ptes(virt_addr_t vma) {
        size_t p4_offset = (vma >> 39) & 0777;
        size_t p3_offset = (vma >> 30) & 0777;
        size_t p2_offset = (vma >> 21) & 0777;
        size_t p1_offset = (vma >> 12) & 0777;

        pte_t *p4e = (pte_t *)P4_BASE + p4_offset;
        pte_t *p3e = (pte_t *)P3_BASE + p4_offset * P1_STRIDE + p3_offset;
        pte_t *p2e = (pte_t *)P2_BASE + p4_offset * P2_STRIDE +
                    p3_offset * P1_STRIDE + p2_offset;
        pte_t *p1e = (pte_t *)P1_BASE + p4_offset * P3_STRIDE +
               p3_offset * P2_STRIDE + p2_offset * P1_STRIDE + p1_offset;

        return (struct ptes){ p4e, p3e, p2e, p1e };
}

bool vmm_check(virt_addr_t vma) {
        static const virt_addr_t half = 0x00007FFFFFFFFFFF;
        if (vma >= ~half)  return true;
        if (vma <= half)   return true;
        return false;
}


struct va_ps {
        virt_addr_t pte_t;
        virt_addr_t page_mask;
};

static phys_addr_t vp_pma(struct va_ps vp, virt_addr_t vma) {
        return (vp.pte_t & vp.page_mask) + (vma & ~vm.page_mask);
}

struct va_ps vmm_resolve_pte(struct ptes ptes, virt_addr_t vma) {
        uintptr_t p4 = *ptes.p4e;
        if (!(p4 & PAGE_PRESENT))  return {VM_NULL}; 

        uintptr_t p3 = *ptes.p3e;
        if (!(p3 & PAGE_PRESENT))  return {VM_NULL};
        if (p3 & PAGE_ISHUGE)
                return { p3, PAGE_MASK_1G };

        uintptr_t p2 = *ptes.p2e;
        if (!(p2 & PAGE_PRESENT))  return {VM_NULL};
        if (p2 & PAGE_ISHUGE)
                return { p2, PAGE_MASK_2M };

        uintptr_t p1 = *ptes.p1e;
        if (!(p1 & PAGE_PRESENT))  return {VM_NULL};
        return { p1, PAGE_MASK_4K };
}

pte_t vmm_pte(virt_addr_t vma) {
        if (!vmm_check(vma)) return VM_NULL;
        struct ptes = vmm_ptes(vma);
        return vmm_resolve_pte(ptes, vma).pte_t;
}

pte_t vmm_fork_pte(virt_addr_t vma) {
        if (!vmm_check(vma)) return VM_NULL;
        struct ptes = vmm_fork_ptes(vma);
        return vmm_resolve_pte(ptes, vma).pte_t;
}

phys_addr_t vmm_phy(virt_addr_t vma) {
        struct va_ps pi = vmm_resolve_pte(vma);
        return vp_pma(pi);
}

phys_addr_t vmm_fork_phy(virt_addr_t vma) {
        struct va_ps pi = vmm_fork_resolve_pte(vma);
        return vp_pma(pi);
}


void make_next_table(pte_t *table_location, uintptr_t flags) {
        phys_addr_t physical = pm_alloc_page();
        *table_location = physical | flags;
}

result_t vmm_map(virt_addr_t vma, phys_addr_t pma, int flags) {
        assert_aligned(vma);
        assert_aligned(pma);
        struct ptes ptes = vmm_ptes(vma);
        int result = vmm_map_ptes(ptes, pma, flags);
        if (result) invlpg(vma);
        return result;
}

result_t vmm_fork_map(virt_addr_t vma, phys_addr_t pma, int flags) {
        assert_aligned(vma);
        assert_aligned(pma);
        struct ptes ptes = vmm_fork_ptes(vma);
        return vmm_map_ptes(ptes, pma, flags);
}

static inline pte_t *table(pte_t *entry) {
        uintptr_t uentry = (uintptr_t)entry;
        return (pte_t *)round_down(uentry, PAGE_SIZE);
}

result_t vmm_map_ptes(struct ptes ptes, phys_addr_t pma, int flags) {
        uintptr_t table_flags = PAGE_WRITEABLE | PAGE_PRESENT;
        if (virtual < 0x800000000000) {
                table_flags |= PAGE_USERMODE; // make explicit?
        }

        if (!(*ptes.p4e & PAGE_PRESENT)) {
                make_next_table(ptes.p4e, table_flags);
                memset(table(ptes.p3e), 0, 0x1000);
        }

        if (*ptes.p3e & PAGE_ISHUGE)  return R_ERROR;
        if (!(*ptes.p3e & PAGE_PRESENT)) {
                make_next_table(ptes.p3e, table_flags);
                memset(table(ptes.p2e), 0, 0x1000);
        }

        if (*ptes.p2e & PAGE_ISHUGE)  return R_ERROR;
        if (!(*ptes.p2e & PAGE_PRESENT)) {
                make_next_table(ptes.p2e, table_flags);
                memset(table(ptes.p1e), 0, 0x1000);
        }

        *ptes.p1e = physical | flags;
        return R_OK;
}

void vmm_unmap(virt_addr_t vma) {
        vmm_map(vma, 0, 0);
}

void vmm_fork_unmap(virt_addr_t vma) {
        vmm_fork_map(vma, 0, 0);
}

// Maps contiguous virtual memory to contiguous physical memory
void vmm_map_range(virt_addr_t vma, phys_addr_t pma, size_t len, int flags) {
        assert_aligned(vma);
        assert_aligned(pma);
        assert_aligned(len);

        if (len <= 0) len = 1;

        for (size_t i = 0; i < len; i++) {
                vmm_map(vma + i * PAGE_SIZE, pma + i * PAGE_SIZE, flags);
        }
}

void vmm_create_unbacked(virt_addr_t vma, int flags) {
        if (vmm_phy(vma) != VM_NULL) {
                return;
        }
        struct ptes ptes = vmm_ptes(vma);
        vmm_create_unbacked_ptes(ptes, flags);
}

void vmm_fork_create_unbacked(virt_addr_t vma, int flags) {
        if (vmm_fork_phy(vma) != VM_NULL) {
                return;
        }
        struct ptes ptes = vmm_fork_ptes(vma);
        vmm_create_unbacked_ptes(ptes, flags);
}

void vmm_create_unbacked_ptes(struct ptes ptes, int flags) {
        assert(flags & PAGE_PRESENT == 0);

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

        vmm_map_ptes(ptes, 0, flags);
}

void vmm_create_unbacked_range(virt_addr_t vma, size_t len, int flags) {
        virt_addr_t first_page = round_down(vma, PAGE_SIZE);
        virt_addr_t end = round_up(vma + len, PAGE_SIZE);
        size_t count = (end - first_page) / PAGE_SIZE;

        for (int i = 0; i < count; i++) {
                vmm_create_unbacked(first_page + i * PAGE_SIZE, flags);
        }
}

void vmm_fork_create_unbacked_range(virt_addr_t vma, size_t len, int flags) {
        virt_addr_t first_page = round_down(vma, PAGE_SIZE);
        virt_addr_t end = round_up(vma + len, PAGE_SIZE);
        size_t count = (end - first_page) / PAGE_SIZE;

        for (int i = 0; i < count; i++) {
                vmm_fork_create_unbacked(first_page + i * PAGE_SIZE, flags);
        }
}

void vmm_set_fork_base(phys_addr_t fork_p4_phy) {
        pte_t *p4 = (pte_t *)P4_BASE;
        pte_t *fork_p4_early = (pte_t *)(P4_BASE + PAGE_SIZE);

        p4[FORK_ENTRY] = fork_p4_phy | PAGE_PRESENT | PAGE_WRITEABLE;
        memset(fork_p4_early, 0, PAGE_SIZE);

        fork_p4_early[REC_ENTRY] = p4[FORK_ENTRY]; // create recursive map
        fork_p4_early[FORK_ENTRY] = p4[FORK_ENTRY]; // create fork map

        for (int i=258; i<512; i++) {
                // copy higher-half mappings
                fork_p4_early[i] = p4[i];
        }

        invlpg(fork_p4_early);
}

void vmm_clear_fork_base() {
        pte_t *p4 = (pte_t *)P4_BASE;
        pte_t *fork_p4_early = (pte_t *)(P4_BASE + PAGE_SIZE);

        fork_p4_early[FORK_ENTRY] = 0;
        p4[FORK_ENTRY] = 0;

        invlpg(fork_p4_early);
}

int vmm_do_page_fault(uintptr_t fault_addr) {
        disable_irqs();
        // 
        // is the page present?
        // is the page COW?
        //
        // perform COW -- on the whole object
        // do access violation things
        // send_immediate_signal_to_self(SIGSEGV);
        //
        enable_irqs();
}

