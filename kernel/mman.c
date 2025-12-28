#include <ng/fs.h>
#include <ng/memmap.h>
#include <ng/mman.h>
#include <ng/syscall.h>
#include <ng/thread.h>
#include <ng/vmm.h>
#include <ng/vmo.h>
#include <errno.h>
#include <stdatomic.h>

// drivers and modules should call this if they want a large amount of virtual
// space available for use over time.
//
// ex: malloc reserves 128MB for it to manage.
//
// there's no way to free currently.

static atomic_uintptr_t kernel_reservable_vma = KERNEL_RESERVABLE_SPACE;

void *vmm_reserve(size_t len) {
	len = ROUND_UP(len, 0x1000);

	uintptr_t res = atomic_fetch_add(&kernel_reservable_vma, len);
	vmm_create_unbacked_range(res, len, PAGE_WRITEABLE);
	return (void *)res;
}

void *vmm_hold(size_t len) {
	len = ROUND_UP(len, 0x1000);

	return (void *)atomic_fetch_add(&kernel_reservable_vma, len);
}

sysret sys_mmap(
	void *addr, size_t len, int prot, int flags, int fd, off_t offset) {

	if (len == 0)
		return -EINVAL;

	len = ROUND_UP(len, PAGE_SIZE);
	uintptr_t base = (uintptr_t)addr;

	// Validate flags - must be either MAP_SHARED or MAP_PRIVATE
	if (!(flags & (MAP_SHARED | MAP_PRIVATE)))
		return -EINVAL;

	// Can't be both SHARED and PRIVATE
	if ((flags & MAP_SHARED) && (flags & MAP_PRIVATE))
		return -EINVAL;

	struct process *p = running_process;
	bool is_fixed = (flags & MAP_FIXED);

	// If MAP_FIXED, validate alignment
	if (is_fixed && (base & (PAGE_SIZE - 1)))
		return -EINVAL;

	// Find address to map at
	uintptr_t map_addr;
	if (is_fixed) {
		// MAP_FIXED: use the specified address
		map_addr = base;

		// Check for overlapping VMAs (simplified: error if any overlap)
		spin_lock(&p->vma_lock);
		struct rbnode *node = rbtree_search_le(&p->vmas, &map_addr);

		// Check VMA before and after for overlaps
		if (node) {
			struct vm_area *vma = container_of(struct vm_area, node, node);
			if (vma->top > map_addr) {
				// Overlaps with VMA before
				spin_unlock(&p->vma_lock);
				return -EINVAL;
			}
		}

		// Check next VMA
		node = rbtree_search_ge(&p->vmas, &map_addr);
		if (node) {
			struct vm_area *vma = container_of(struct vm_area, node, node);
			if (map_addr + len > vma->base) {
				// Overlaps with VMA after
				spin_unlock(&p->vma_lock);
				return -EINVAL;
			}
		}
		spin_unlock(&p->vma_lock);

	} else {
		// Find a gap in the address space
		map_addr = vma_find_gap(p, len, USER_MMAP_BASE, USER_STACK);
		if (map_addr == 0)
			return -ENOMEM; // No space available
	}

	// Create the VMO
	struct vmo *vmo = nullptr;
	if (flags & MAP_ANONYMOUS) {
		vmo = vmo_new_anon(len);
	} else {
		struct file *ofd = get_file(fd);
		if (!ofd)
			return -EBADF;
		struct vnode *vnode = ofd->vnode;
		if (vnode->type != FT_NORMAL)
			return -ENODEV;
		vmo = vmo_new_file(vnode, offset, len);
	}
	if (!vmo)
		return -ENOMEM;

	// Map the VMA
	int map_ret = vma_map(p, map_addr, len, prot, flags, vmo, 0);
	vmo_unref(vmo);
	if (map_ret != 0)
		return map_ret;

	return map_addr;
}

sysret sys_munmap(void *addr, size_t length) {
	uintptr_t base = (uintptr_t)addr;

	// Validate alignment
	if (base & (PAGE_SIZE - 1))
		return -EINVAL;

	if (length == 0)
		return -EINVAL;

	length = ROUND_UP(length, PAGE_SIZE);
	uintptr_t top = base + length;

	// Check for overflow
	if (top < base)
		return -EINVAL;

	struct process *p = running_process;

	spin_lock(&p->vma_lock);

	// Find VMA at base address
	struct rbnode *node = rbtree_search(&p->vmas, &base);
	if (!node) {
		spin_unlock(&p->vma_lock);
		return -EINVAL; // No VMA at this address
	}

	struct vm_area *vma = container_of(struct vm_area, node, node);

	// Must match VMA boundaries exactly (no splitting)
	if (vma->base != base || vma->top != top) {
		spin_unlock(&p->vma_lock);
		return -EINVAL;
	}

	// Remove from tree (returns node without freeing)
	rbtree_remove(&p->vmas, &base);

	spin_unlock(&p->vma_lock);

	// Cleanup
	vmm_unmap_range(vma->base, vma->top - vma->base);
	vmo_unref(vma->vmo);
	free(vma);

	return 0;
}
