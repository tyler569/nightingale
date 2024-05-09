#include "ng/mem.h"
#include "stdatomic.h"
#include "string.h"
#include "sys/cdefs.h"
#include "x86_64.h"

typedef uint64_t pte_t;

#define PTE_PRESENT (1 << 0)
#define PTE_WRITE (1 << 1)
#define PTE_USER (1 << 2)
#define PTE_PWT (1 << 3)
#define PTE_PCD (1 << 4)
#define PTE_ACCESSED (1 << 5)
#define PTE_DIRTY (1 << 6)
#define PTE_PSE (1 << 7)
#define PTE_GLOBAL (1 << 8)
#define PTE_PAT (1 << 12)
#define PTE_NX (1ULL << 63)

#define PTE_ADDR(pte) ((pte) & 0x000FFFFFFFFFF000)
#define PTE(pte) (pte_t *)(direct_map_of(PTE_ADDR(pte)))

#define PML4_SHIFT 39
#define PML4_MASK 0x1FF

#define PDP_SHIFT 30
#define PDP_MASK 0x1FF

#define PD_SHIFT 21
#define PD_MASK 0x1FF

#define PT_SHIFT 12
#define PT_MASK 0x1FF

#define PAGE_SIZE 4096

extern uintptr_t KERNEL_END;
static _Atomic uintptr_t vm_alloc_base = (uintptr_t)&KERNEL_END;

uintptr_t get_vm_root() {
	uintptr_t root;
	asm volatile("mov %%cr3, %0" : "=r"(root));
	return root;
}

void set_vm_root(uintptr_t root) {
	asm volatile("mov %0, %%cr3" : : "r"(root));
}

uintptr_t alloc_table() {
	uintptr_t page = alloc_page();
	void *table = (void *)direct_map_of(page);
	memset(table, 0, PAGE_SIZE);
	return page;
}

void add_vm_mapping(uintptr_t root, uintptr_t virt, uintptr_t phys, int flags) {
	pte_t *pml4 = PTE(root);
	pte_t *pdp;
	pte_t *pd;
	pte_t *pt;
	pte_t *pte;
	int table_flags = PTE_PRESENT | PTE_WRITE | (flags & PTE_USER);

	pte_t pml4e = pml4[virt >> PML4_SHIFT & PML4_MASK];
	if (!(pml4e & PTE_PRESENT)) {
		uintptr_t pdp_page = alloc_table();
		pml4[virt >> PML4_SHIFT & PML4_MASK] = pdp_page | table_flags;
		pdp = PTE(pdp_page);
	} else
		pdp = PTE(pml4e);

	pte_t pdpe = pdp[(virt >> PDP_SHIFT) & PDP_MASK];
	if (!(pdpe & PTE_PRESENT)) {
		uintptr_t pd_page = alloc_table();
		pdp[(virt >> PDP_SHIFT) & PDP_MASK] = pd_page | table_flags;
		pd = PTE(pd_page);
	} else
		pd = PTE(pdpe);

	pte_t pde = pd[(virt >> PD_SHIFT) & PD_MASK];
	if (!(pde & PTE_PRESENT)) {
		uintptr_t pt_page = alloc_table();
		pd[(virt >> PD_SHIFT) & PD_MASK] = (uintptr_t)pt_page | table_flags;
		pt = PTE(pt_page);
	} else
		pt = PTE(pde);

	pte = &pt[(virt >> PT_SHIFT) & PT_MASK];
	*pte = phys | flags;
}

uintptr_t resolve_vm_mapping(uintptr_t root, uintptr_t virt) {
	pte_t *pml4 = PTE(root);
	pte_t *pdp;
	pte_t *pd;
	pte_t *pt;
	pte_t *pte;

	pte_t pml4e = pml4[virt >> PML4_SHIFT & PML4_MASK];
	if (!(pml4e & PTE_PRESENT))
		return 0;

	pdp = PTE(pml4e);

	pte_t pdpe = pdp[(virt >> PDP_SHIFT) & PDP_MASK];
	if (!(pdpe & PTE_PRESENT))
		return 0;

	pd = PTE(pdpe);

	pte_t pde = pd[(virt >> PD_SHIFT) & PD_MASK];
	if (!(pde & PTE_PRESENT))
		return 0;

	pt = PTE(pde);

	pte = &pt[(virt >> PT_SHIFT) & PT_MASK];
	if (!(*pte & PTE_PRESENT))
		return 0;

	return PTE_ADDR(*pte) | (virt & 0xFFF);
}

uintptr_t alloc_kernel_vm(size_t len) {
	len = ALIGN_UP(len, PAGE_SIZE);
	return atomic_fetch_add(&vm_alloc_base, len);
}

uintptr_t new_page_table() {
	uintptr_t root = alloc_table();
	pte_t *pml4 = PTE(root);

	pte_t *current_root = PTE(get_vm_root());

	// Copy all higher-half (kernel) mappings
	for (int i = 256; i < 512; i++)
		pml4[i] = current_root[i];

	return root;
}

void destroy_page_table(uintptr_t) {
	// todo
}
