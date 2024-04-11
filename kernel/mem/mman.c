#include <ng/common.h>
#include <ng/fs.h>
#include <ng/memmap.h>
#include <ng/mman.h>
#include <ng/syscall.h>
#include <ng/thread.h>
#include <ng/vmm.h>
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
	len = ROUND_UP(len, 0x1000);

	// TODO:
	// This is a very dumb simple bump allocator just being made
	// to make it possible for me to make a dumb simple bump
	// allocator for user mode.
	//
	// It doesn't do a lot of mmap things at all.

	if (addr != nullptr)
		return -ETODO;
	if (!(flags & MAP_PRIVATE))
		return -ETODO;

	uintptr_t new_alloc = running_process->mmap_base;
	user_map(new_alloc, new_alloc + len);
	running_process->mmap_base += len;

	if (!(flags & MAP_ANONYMOUS)) {
		struct file *ofd = get_file(fd);
		if (!ofd)
			return -EBADF;
		struct vnode *vnode = ofd->vnode;
		if (vnode->type != FT_NORMAL)
			return -ENODEV;
		size_t to_copy = MIN(len, vnode->len);
		memcpy((void *)new_alloc, vnode->data, to_copy);
	}

	return new_alloc;
}

sysret sys_munmap(void *addr, size_t length) {
	// nop, TODO
	return 0;
}
