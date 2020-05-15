
// #define DEBUG
#include <basic.h>
#include <ng/debug.h>
#include <ng/mman.h>
#include <ng/syscall.h>
#include <ng/thread.h>
#include <ng/vmm.h>
#include <ng/memmap.h>
#include <ng/fs.h>
#include <errno.h>
#include <stdio.h>

// drivers and modules should call this if they want a large amount of virtual
// space available for use over time.
//
// ex: malloc reserves 128MB for it to manage.
//
// there's no way to free currently.

char *kernel_reservable_vma = (char *)KERNEL_RESERVABLE_SPACE;

void *vmm_reserve(size_t len) {
        len = round_up(len, 0x1000);

        void *res = kernel_reservable_vma;
        // printf("RESERVING RANGE %p + %lx\n", res, len);
        kernel_reservable_vma += len;

        vmm_create_unbacked_range((uintptr_t)res, len, PAGE_WRITEABLE);
        return res;
}

void *high_vmm_reserve(size_t len) {
        return vmm_reserve(len);
}

sysret sys_mmap(void *addr, size_t len, int prot,
                int flags, int fd, off_t offset) {
        len = round_up(len, 0x1000);

        // TODO:
        // This is a very dumb simple bump allocator just being made
        // to make it possible for me to make a dumb simple bump
        // allocator for user mode.
        //
        // It doesn't do a lot of mmap things at all.

        if (addr != NULL) {
                return -ETODO;
        }

        if (!(flags & MAP_ANONYMOUS)) {
                return -ETODO;
        }

        uintptr_t new_alloc = running_process->mmap_base;

        vmm_create_unbacked_range(new_alloc, len,
                                  PAGE_WRITEABLE | PAGE_USERMODE);

        running_process->mmap_base += len;

        return new_alloc;
}

sysret sys_munmap(void *addr, size_t length) {
        // nop, TODO
        return 0;
}
