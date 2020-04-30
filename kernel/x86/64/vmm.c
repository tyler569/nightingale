
// #define DEBUG
#include <basic.h>
#include <ng/debug.h>
#include <ng/panic.h>
#include <ng/pmm.h>
#include <ng/vmm.h>
#include <ng/x86/64/cpu.h>
#include <nc/list.h>
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

#define assert_aligned(v) assert((v & PAGE_OFFSET_4K) == 0);

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

struct ptes {
        pte_t *p4e, *p3e, *p2e, *p1e;
};

#define SIZEOF_ENTRY sizeof(uintptr_t)
#define P1_STRIDE 0x1000 / SIZEOF_ENTRY
#define P2_STRIDE 0x200000 / SIZEOF_ENTRY
#define P3_STRIDE 0x40000000 / SIZEOF_ENTRY

void print_ptes(struct ptes *ptes) {
        printf("ptes {");
        printf("%p, ", ptes->p4e);
        printf("%p, ", ptes->p3e);
        printf("%p, ", ptes->p2e);
        printf("%p", ptes->p1e);
        printf("}");
}

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

        pte_t *p4e = (pte_t *)FORK_P4_BASE + p4_offset;
        pte_t *p3e = (pte_t *)FORK_P3_BASE + p4_offset * P1_STRIDE + p3_offset;
        pte_t *p2e = (pte_t *)FORK_P2_BASE + p4_offset * P2_STRIDE +
                    p3_offset * P1_STRIDE + p2_offset;
        pte_t *p1e = (pte_t *)FORK_P1_BASE + p4_offset * P3_STRIDE +
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
        virt_addr_t pte;
        virt_addr_t page_mask;
};

static phys_addr_t vp_pma(struct va_ps vp, virt_addr_t vma) {
        if (vp.pte == VM_NULL || (vp.pte & PAGE_PRESENT) == 0)  return VM_NULL;
        return (vp.pte & vp.page_mask) + (vma & ~vp.page_mask);
}

struct va_ps vmm_resolve_pte(struct ptes ptes) {
        uintptr_t p3x = *ptes.p4e;
        if (!(p3x & PAGE_PRESENT))  return (struct va_ps){ VM_NULL, 0 }; 

        uintptr_t p2x = *ptes.p3e;
        if (!(p2x & PAGE_PRESENT))  return (struct va_ps){ VM_NULL, 0 };
        if (p2x & PAGE_ISHUGE)
                return (struct va_ps){ p2x, PAGE_MASK_1G };

        uintptr_t p1x = *ptes.p2e;
        if (!(p1x & PAGE_PRESENT))  return (struct va_ps){ VM_NULL, 0 };
        if (p1x & PAGE_ISHUGE)
                return (struct va_ps){ p1x, PAGE_MASK_2M };

        uintptr_t page = *ptes.p1e;
        return (struct va_ps){ page, PAGE_MASK_4K };
}

pte_t vmm_pte(virt_addr_t vma) {
        if (!vmm_check(vma)) return VM_NULL;
        struct ptes ptes = vmm_ptes(vma);
        return vmm_resolve_pte(ptes).pte;
}

pte_t vmm_fork_pte(virt_addr_t vma) {
        if (!vmm_check(vma)) return VM_NULL;
        struct ptes ptes = vmm_fork_ptes(vma);
        return vmm_resolve_pte(ptes).pte;
}

phys_addr_t vmm_phy(virt_addr_t vma) {
        if (!vmm_check(vma)) return VM_NULL;
        struct ptes ptes = vmm_ptes(vma);
        struct va_ps pi = vmm_resolve_pte(ptes);
        return vp_pma(pi, vma);
}

phys_addr_t vmm_fork_phy(virt_addr_t vma) {
        if (!vmm_check(vma)) return VM_NULL;
        struct ptes ptes = vmm_ptes(vma);
        struct va_ps pi = vmm_resolve_pte(ptes);
        return vp_pma(pi, vma);
}


void make_next_table(pte_t *table_location, uintptr_t flags) {
        phys_addr_t physical = pm_alloc_page();
        printf("make next table %p -> %p\n", table_location, physical);
        *table_location = physical | flags;
}

static inline pte_t *table(pte_t *entry) {
        uintptr_t uentry = (uintptr_t)entry;
        pte_t *p = (pte_t *)round_down(uentry, PAGE_SIZE);
        printf("make_next_table memset pte is %p\n", p);
        
        uintptr_t up = (uintptr_t)p;
        return p;
}

int vmm_map_ptes(struct ptes ptes, phys_addr_t pma, int flags) {
        uintptr_t table_flags = PAGE_WRITEABLE | PAGE_PRESENT;
        if (flags & PAGE_USERMODE || pma & PAGE_USERMODE /* HACK */) {
                table_flags |= PAGE_USERMODE;
        }

        if (!(*ptes.p4e & PAGE_PRESENT)) {
                make_next_table(ptes.p4e, table_flags);
                memset(table(ptes.p3e), 0, PAGE_SIZE);
        }

        if (*ptes.p3e & PAGE_ISHUGE)  return 0;
        if (!(*ptes.p3e & PAGE_PRESENT)) {
                make_next_table(ptes.p3e, table_flags);
                memset(table(ptes.p2e), 0, PAGE_SIZE);
        }

        if (*ptes.p2e & PAGE_ISHUGE)  return 0;
        if (!(*ptes.p2e & PAGE_PRESENT)) {
                make_next_table(ptes.p2e, table_flags);
                memset(table(ptes.p1e), 0, PAGE_SIZE);
        }

        *ptes.p1e = pma | flags;
        return 1;
}

int vmm_map(virt_addr_t vma, phys_addr_t pma, int flags) {
        assert_aligned(vma);
        // assert_aligned(pma); // can contain flags sometimes

        // printf("vmm_map %p -> phy: %p (%i)\n", vma, pma, flags);

        struct ptes ptes = vmm_ptes(vma);
        int result = vmm_map_ptes(ptes, pma, flags);
        if (result) invlpg(vma);
        return result;
}

int vmm_fork_map(virt_addr_t vma, phys_addr_t pma, int flags) {
        assert_aligned(vma);
        // assert_aligned(pma); // can contain flags sometimes
        struct ptes ptes = vmm_fork_ptes(vma);
        return vmm_map_ptes(ptes, pma, flags);
}

int vmm_unmap(virt_addr_t vma) {
        return vmm_map(vma, 0, 0);
}

int vmm_fork_unmap(virt_addr_t vma) {
        return vmm_fork_map(vma, 0, 0);
}

// Maps contiguous virtual memory to contiguous physical memory
void vmm_map_range(virt_addr_t vma, phys_addr_t pma, size_t len, int flags) {
        assert_aligned(vma);
        assert_aligned(pma);
        assert_aligned(len);

        if (len <= 0) len = 1;

        for (size_t i = 0; i < len; i += PAGE_SIZE) {
                vmm_map(vma + i, pma + i, flags);
        }
}

void vmm_fork_map_range(virt_addr_t vma, phys_addr_t pma, size_t len, int flags) {
        assert_aligned(vma);
        assert_aligned(pma);
        assert_aligned(len);

        if (len <= 0) len = 1;

        for (size_t i = 0; i < len; i += PAGE_SIZE) {
                int res = vmm_fork_map(vma + i, pma + i, flags);
                assert(res);
        }
}

/*
 * Unmaps a range of virtual addresses, releasing the physical memory backing
 * the range to the page allocator.
 */
void vmm_unmap_range_free(virt_addr_t base, size_t len) {
        assert_aligned(base);
        assert_aligned(len);

        int pages = len / PAGE_SIZE;

        for (int i=0; i<pages; i++) {
                pte_t pte = vmm_pte(base + i * PAGE_SIZE);
                if (pte & PAGE_PRESENT) {
                        phys_addr_t page = pte & PAGE_MASK_4K;
                        pm_free_page(page);
                }
        }
}

void vmm_create_unbacked(virt_addr_t vma, int flags) {
        if (vmm_phy(vma) != VM_NULL)  return;
        assert(!(flags & PAGE_PRESENT));

        vmm_map(vma, 0, flags | PAGE_UNBACKED);
}

void vmm_fork_create_unbacked(virt_addr_t vma, int flags) {
        assert(!(flags & PAGE_PRESENT));

        vmm_fork_map(vma, 0, flags | PAGE_UNBACKED);
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

/* THESE ARE NOT X64 SPECIFIC!!! */

void vmm_fork_copyfrom(virt_addr_t fork_base, virt_addr_t this_base, int pages) {
        assert_aligned(this_base);
        assert_aligned(fork_base);

        for (int i=0; i<pages; i++) {
                virt_addr_t tp = this_base + i * PAGE_SIZE;
                virt_addr_t fp = fork_base + i * PAGE_SIZE;

                pte_t tp_pte = vmm_pte(tp);
                vmm_fork_map(fp, tp_pte, 0); // "tp_pte" includes flags.
        }
}

void vmm_remap(virt_addr_t base, virt_addr_t top, int vm_flags) {
        assert_aligned(base);
        assert_aligned(top);
        assert(vm_flags == VM_COW);

        for (virt_addr_t p = base; p != top; p += PAGE_SIZE) {
                pte_t p_pte = vmm_pte(p);
                pte_t new_pte = (p_pte & ~PAGE_WRITEABLE) | PAGE_COPYONWRITE;
                vmm_map(p, new_pte, 0); // "new_pte" includes flags.
        }
}

/* END NOT X64 SPECIFIC */

/*
 * This is kinda the linux __function / function thing, how much do I
 * hate this?
 */
pte_t *__vmm_set_fork_base(phys_addr_t fork_p4_phy) {
        mutex_await(&fork_mutex);

        pte_t *p4 = (pte_t *)P4_BASE;
        pte_t *fork_p4_early = (pte_t *)(P4_BASE + PAGE_SIZE);

        p4[FORK_ENTRY] = fork_p4_phy | PAGE_PRESENT | PAGE_WRITEABLE;
        memset(fork_p4_early, 0, PAGE_SIZE);

        fork_p4_early[REC_ENTRY] = p4[FORK_ENTRY]; // create recursive map
        fork_p4_early[FORK_ENTRY] = p4[FORK_ENTRY]; // create fork map

        return fork_p4_early;
}

void vmm_set_fork_base(phys_addr_t fork_p4_phy) {
        pte_t *fork_p4_early = __vmm_set_fork_base(fork_p4_phy);
        pte_t *p4 = (pte_t *)P4_BASE;

        for (int i=258; i<512; i++) {
                // copy higher-half mappings
                fork_p4_early[i] = p4[i];
        }

        invlpg((uintptr_t)fork_p4_early);
}

void vmm_set_fork_base_kernel(phys_addr_t fork_p4_phy) {
        pte_t *fork_p4_early = __vmm_set_fork_base(fork_p4_phy);

        invlpg((uintptr_t)fork_p4_early);
}

void vmm_clear_fork_base() {
        pte_t *p4 = (pte_t *)P4_BASE;
        pte_t *fork_p4_early = (pte_t *)(P4_BASE + PAGE_SIZE);

        fork_p4_early[FORK_ENTRY] = 0;
        p4[FORK_ENTRY] = 0;

        invlpg((uintptr_t)fork_p4_early);

        mutex_unlock(&fork_mutex);
}

void vmm_set_pgtable(phys_addr_t p4base) {
        assert(p4base != 0);

        set_vm_root(p4base);
}

int vmm_do_page_fault(virt_addr_t fault_addr) {
        disable_irqs();

        pte_t pte = vmm_pte(fault_addr);
        printf("PAGE FAULT! %p\n", pte);

        if (pte == VM_NULL) {
                void break_point(void);
                break_point();
                enable_irqs();
                return 0; // Non-present page. Continue fault
        }

        if (!(pte & PAGE_PRESENT) && (pte & PAGE_UNBACKED)) {
                phys_addr_t new_page = pm_alloc_page();
                printf("vmm: backing unbacked at %zx with %zx\n", fault_addr, new_page);
                virt_addr_t base = fault_addr & PAGE_MASK_4K;
                vmm_unmap(base);
                pte_t new_pte = (pte & ~PAGE_UNBACKED) | new_page | PAGE_PRESENT;
                vmm_map(base, new_pte, 0); // new_pte includes flags;

                enable_irqs();
                return 1; // Do not continue fault
        }

        if (pte & PAGE_COPYONWRITE) { // && error_code & VM_FAULT_WRITE
                struct vm_map_object *mo = vm_with(fault_addr);
                struct vm_object *vmo = mo->object;

                virt_addr_t temp = vm_alloc(vmo->pages * PAGE_SIZE);
                void *temp_mem = (void *)temp;
                void *src_mem = (void *)vmo->base;

                memcpy(temp_mem, src_mem, vmo->pages * PAGE_SIZE);
                vm_user_unmap(mo);
                vmo = mo->object; // MO CHANGES IN UNMAP
                vm_user_remap(mo);

                /* Questionable assertion:
                 *
                 * I belive the invlpg's in vmm_map (called in vmm_unmap) 
                 * are enough to make this safe immeditately with no futher
                 * jugging needed.
                 */

                memcpy(src_mem, temp_mem, vmo->pages * PAGE_SIZE);
                enable_irqs();
                return 1; // Do not continue fault
        }
        enable_irqs();
        return 0; // No case matched. Continue fault
}

/*
void vmm_destroy_level(int level, pte_t *pte) {
}
*/

void vmm_destroy_tree(phys_addr_t pgtable_root) {
        return; // TODO;
}

