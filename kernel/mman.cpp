#include <ng/common.h>
#include <ng/fs.h>
#include <ng/memmap.h>
#include <ng/mman.h>
#include <ng/sync.h>
#include <ng/syscall.h>
#include <ng/syscalls.h>
#include <ng/thread.h>
#include <ng/vmm.h>
#include <nx/spinlock.h>

// drivers and modules should call this if they want a large amount of virtual
// space available for use over time.
//
// ex: malloc reserves 128MB for it to manage.
//
// there's no way to free currently.

static nx::spinlock reserve_lock;
static char *kernel_reservable_vma = (char *)KERNEL_RESERVABLE_SPACE;

void *vmm_reserve(size_t len)
{
    len = ROUND_UP(len, 0x1000);

    reserve_lock.lock();
    void *res = kernel_reservable_vma;
    // printf("RESERVING RANGE %p + %lx\n", res, len);
    kernel_reservable_vma += len;
    reserve_lock.unlock();

    vmm_create_unbacked_range((uintptr_t)res, len, PAGE_WRITEABLE);
    return res;
}

void *vmm_hold(size_t len)
{
    len = ROUND_UP(len, 0x1000);

    reserve_lock.lock();
    void *res = kernel_reservable_vma;
    kernel_reservable_vma += len;
    reserve_lock.unlock();

    return res;
}

sysret sys_mmap(
    void *addr, size_t len, int prot, int flags, int fd, off_t offset)
{
    struct file *file = get_file(fd);
    if (file && file->ops && file->ops->mmap) {
        return (sysret)file->ops->mmap(file, addr, len, prot, flags, offset);
    }

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

    uintptr_t new_alloc = allocate_mmap_space(len);
    user_map(new_alloc, new_alloc + len);

    if (!(flags & MAP_ANONYMOUS)) {
        struct file *ofd = get_file(fd);
        if (!ofd)
            return -EBADF;
        struct inode *inode = ofd->inode;
        if (inode->type != FT_NORMAL)
            return -ENODEV;
        size_t to_copy = MIN(len, inode->len);
        memcpy((void *)new_alloc, inode->data, to_copy);
    }

    return new_alloc;
}

sysret sys_munmap(void *addr, size_t length)
{
    // nop, TODO
    return 0;
}
