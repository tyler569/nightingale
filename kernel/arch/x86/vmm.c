#include <assert.h>
#include <ng/common.h>
#include <ng/pmm.h>
#include <ng/syscall.h>
#include <ng/thread.h>
#include <ng/vmm.h>
#include <string.h>

#define VMM_MAP_BASE 0xFFFF800000000000

static size_t vm_offset(virt_addr_t vma, int level) {
	assert(level > 0 && level < 5);
	return (vma >> (12 + 9 * (level - 1))) & 0777;
}

static bool is_unbacked(uintptr_t pte) {
	return (!(pte & PAGE_PRESENT) && (pte & PAGE_UNBACKED));
}

void reset_tlb() {
	uintptr_t cr3;
	asm volatile("mov %%cr3, %0" : "=a"(cr3));
	asm volatile("mov %0, %%cr3" : : "a"(cr3));
}

static uintptr_t make_next_table_int(uintptr_t *pte_ptr, bool kernel) {
	phys_addr_t next_table = pm_alloc();
	memset((void *)(next_table + VMM_MAP_BASE), 0, PAGE_SIZE);
	uintptr_t next_pte = next_table | PAGE_PRESENT | PAGE_WRITEABLE;
	if (!kernel)
		next_pte |= PAGE_USERMODE;
	*pte_ptr = next_pte;
	return next_pte;
}

static uintptr_t *vmm_pte_ptr_int(
	virt_addr_t vma, phys_addr_t root, int level, bool create) {
	size_t offset = vm_offset(vma, level);
	uintptr_t *table = (uintptr_t *)(root + VMM_MAP_BASE);
	uintptr_t *pte_ptr = &table[offset];
	if (level == 1)
		return pte_ptr;
	uintptr_t pte = *pte_ptr;

	if (!(pte & PAGE_PRESENT)) {
		if (create) {
			pte = make_next_table_int(pte_ptr, vma > 0xFFFF000000000000);
		} else {
			return nullptr;
		}
	}
	assert(!(pte & PAGE_ISHUGE)); // no support at this time
	return vmm_pte_ptr_int(vma, pte & PAGE_ADDR_MASK, level - 1, create);
}

phys_addr_t vmm_virt_to_phy(virt_addr_t vma) {
	phys_addr_t vm_root = running_process->vm_root;
	uintptr_t *pte_ptr = vmm_pte_ptr_int(vma, vm_root, 4, false);
	if (!pte_ptr)
		return -1;
	uintptr_t pte = *pte_ptr;
	if (!(pte & PAGE_PRESENT))
		return -1;
	return (pte & PAGE_ADDR_MASK) + (vma & PAGE_OFFSET_4K);
}

uintptr_t *vmm_pte_ptr(virt_addr_t vma) {
	phys_addr_t vm_root = running_process->vm_root;
	return vmm_pte_ptr_int(vma, vm_root, 4, false);
}

static uintptr_t *vmm_pte_ptr_next(
	virt_addr_t vma, uintptr_t *pte_ptr, phys_addr_t vm_root, bool create) {
	// Basic implementation that just scrubs through a single P1 table before
	// re-resolving. The real idea situation here would be something recursive,
	// scrubbing the whole table with a callback probably, but I expect
	// this to be a ~512x speedup and that's likely better than the callback
	// solution anyway.

	assert(pte_ptr);
	if ((vma & (0777 << 12)) == 0) {
		return vmm_pte_ptr_int(vma, vm_root, 4, create);
	} else {
		return pte_ptr + 1;
	}
}

static bool vmm_map_range_int(
	virt_addr_t vma, phys_addr_t pma, size_t len, int flags, bool force) {
	phys_addr_t vm_root = running_process->vm_root;

	virt_addr_t page = vma;
	uintptr_t *pte_ptr = vmm_pte_ptr_int(page, vm_root, 4, true);

	do {
		if (!pte_ptr)
			return false;
		if (*pte_ptr && !force)
			goto next;
		uintptr_t old_page = *pte_ptr & PAGE_ADDR_MASK;

		*pte_ptr = (pma & PAGE_MASK_4K) | flags;
		invlpg(page);

		if (pma == 0 && flags == 0 && old_page) { // unmap
			pm_decref(old_page);
		}

	next:
		page += PAGE_SIZE;
		if (flags & PAGE_PRESENT)
			pma += PAGE_SIZE;
		pte_ptr = vmm_pte_ptr_next(page, pte_ptr, vm_root, true);
	} while (page < vma + len);

	return true;
}

static bool vmm_map_int(
	virt_addr_t vma, phys_addr_t pma, int flags, bool force) {
	return vmm_map_range_int(vma, pma, PAGE_SIZE, flags, force);
}

bool vmm_map(virt_addr_t vma, phys_addr_t pma, int flags) {
	return vmm_map_int(vma, pma, flags | PAGE_PRESENT, false);
}

void vmm_map_range(virt_addr_t vma, phys_addr_t pma, size_t len, int flags) {
	assert((vma & PAGE_OFFSET_4K) == 0);
	assert((pma & PAGE_OFFSET_4K) == 0);
	len = ROUND_UP(len, PAGE_SIZE);
	vmm_map_range_int(vma, pma, len, flags | PAGE_PRESENT, false);
}

void vmm_create_unbacked(virt_addr_t vma, int flags) {
	vmm_map_int(vma, 0, flags | PAGE_UNBACKED, false);
}

void vmm_create_unbacked_range(virt_addr_t vma, size_t len, int flags) {
	assert((vma & PAGE_OFFSET_4K) == 0);
	len = ROUND_UP(len, PAGE_SIZE);
	vmm_map_range_int(vma, 0, len, flags | PAGE_UNBACKED, false);
}

bool vmm_unmap(virt_addr_t vma) { return vmm_map_int(vma, 0, 0, true); }

void vmm_unmap_range(virt_addr_t vma, size_t len) {
	assert((vma & PAGE_OFFSET_4K) == 0);
	len = ROUND_UP(len, PAGE_SIZE);
	vmm_map_range_int(vma, 0, len, 0, true);
}

static void vmm_copy(
	virt_addr_t vma, phys_addr_t new_root, enum vmm_copy_op op) {
	uintptr_t *pte_ptr = vmm_pte_ptr(vma);
	assert(pte_ptr);
	uintptr_t pte = *pte_ptr;
	phys_addr_t page = pte & PAGE_MASK_4K;
	phys_addr_t new_page;
	uintptr_t *new_ptr = vmm_pte_ptr_int(vma, new_root, 4, true);
	assert(new_ptr);

	if (is_unbacked(pte)) {
		*new_ptr = pte;
		return;
	}

	switch (op) {
	case COPY_COW:
		*pte_ptr &= ~PAGE_WRITEABLE;
		*pte_ptr |= PAGE_COPYONWRITE;
		*new_ptr = *pte_ptr;
		invlpg(vma);
		pm_incref(page);
		break;
	case COPY_SHARED:
		*new_ptr = pte;
		pm_incref(page);
		break;
	case COPY_EAGER:
		new_page = pm_alloc();
		memcpy((void *)vma, (void *)(new_page + VMM_MAP_BASE), PAGE_SIZE);
		*new_ptr = (pte & PAGE_FLAGS_MASK) | new_page;
		break;
	default:
		panic("illegal vm_copy operation");
	}
}

static void vmm_copy_region(virt_addr_t base, virt_addr_t top,
	phys_addr_t new_root, enum vmm_copy_op op) {
	assert((base & PAGE_OFFSET_4K) == 0);
	assert((top & PAGE_OFFSET_4K) == 0);

	if (base == 0)
		return;

	for (size_t page = base; page < top; page += PAGE_SIZE) {
		vmm_copy(page, new_root, op);
	}
}

phys_addr_t vmm_create() {
	phys_addr_t new_vm_root = pm_alloc();
	uintptr_t *new_root_ptr = (uintptr_t *)(new_vm_root + VMM_MAP_BASE);

	phys_addr_t vm_root = running_process->vm_root;
	uintptr_t *vm_root_ptr = (uintptr_t *)(vm_root + VMM_MAP_BASE);

	// copy the top half to the new table;
	memcpy(new_root_ptr + 256, vm_root_ptr + 256, 256 * sizeof(uintptr_t));
	memset(new_root_ptr, 0, 256 * sizeof(uintptr_t));

	return new_vm_root;
}

phys_addr_t vmm_fork(struct process *child, struct process *parent) {
	phys_addr_t new_vm_root = vmm_create();

	struct mm_region *regions = &parent->mm_regions[0];
	for (size_t i = 0; i < NREGIONS; i++) {
		vmm_copy_region(regions[i].base, regions[i].top, new_vm_root, COPY_COW);
	}
	memcpy(&child->mm_regions, &parent->mm_regions,
		sizeof(struct mm_region) * NREGIONS);

	return new_vm_root;
}

static void vmm_destroy_tree_int(phys_addr_t root, int level) {
	size_t top = 512;
	if (level == 4)
		top = 256;
	uintptr_t *root_ptr = (uintptr_t *)(root + VMM_MAP_BASE);

	for (size_t i = 0; i < top; i++) {
		uint64_t pte = root_ptr[i];
		if ((pte & PAGE_PRESENT) && level > 1) {
			vmm_destroy_tree_int(pte & PAGE_ADDR_MASK, level - 1);
		}
		// printf("%.*s %p\n", level, "    ", pte);
		if (pte & PAGE_PRESENT)
			pm_free(pte & PAGE_ADDR_MASK);
		root_ptr[i] = 0;
	}
}

void vmm_destroy_tree(phys_addr_t root) {
	vmm_destroy_tree_int(root, 4);
	pm_free(root);
}

void vmm_early_init() {
	// hhstack_guard_page = 0
	// remap ro_begin to ro_end read-only
}

phys_addr_t vmm_resolve(virt_addr_t address) {
	uintptr_t *ptr = vmm_pte_ptr(address);
	return *ptr & PAGE_ADDR_MASK;
}

enum fault_result vmm_do_page_fault(
	virt_addr_t fault_addr, enum x86_fault reason) {
	uintptr_t pte, phy, cur, flags, new_flags;
	uintptr_t *pte_ptr = vmm_pte_ptr(fault_addr);

	// printf("page fault %p %#02x\n", fault_addr, reason);

	if (!pte_ptr)
		return FAULT_CRASH;
	pte = *pte_ptr;
	if (pte == 0)
		return FAULT_CRASH;

	if (reason & F_RESERVED)
		return FAULT_CRASH;
	if (reason & F_RESERVED)
		return FAULT_CRASH;

	if (is_unbacked(pte)) {
		phy = pm_alloc();
		*pte_ptr &= PAGE_FLAGS_MASK;
		*pte_ptr |= phy | PAGE_PRESENT;
		return FAULT_CONTINUE;
	}

	if ((pte & PAGE_COPYONWRITE) && (reason & F_WRITE)) {
		phy = pm_alloc();
		cur = pte & PAGE_ADDR_MASK;
		flags = pte & PAGE_FLAGS_MASK;

		memcpy((void *)(phy + VMM_MAP_BASE), (void *)(cur + VMM_MAP_BASE),
			PAGE_SIZE);
		pm_decref(cur);

		new_flags = flags & ~(PAGE_COPYONWRITE | PAGE_ACCESSED | PAGE_DIRTY);
		*pte_ptr = phy | new_flags | PAGE_WRITEABLE;
		invlpg(fault_addr);
		return FAULT_CONTINUE;
	}

	if (pte & PAGE_STACK_GUARD) {
		printf("Warning! Page fault in page marked stack guard!\n");
		return FAULT_CRASH;
	}

	return FAULT_CRASH;
}
