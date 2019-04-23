
// #define DEBUG
#include <ng/basic.h>
#include <ng/debug.h>
#include <ng/mman.h>
#include <ng/print.h>
#include <ng/syscall.h>
#include <ng/vmm.h>
#include <arch/memmap.h>
#include <fs/vfs.h>

uintptr_t mmap_base = USER_MMAP_BASE;

struct syscall_ret sys_mmap(void *addr, size_t len, int prot, int flags, int fd,
                            off_t offset) {

        // TODO:
        // This is a very dumb simple bump allocator just being made
        // to make it possible for me to make a dumb simple bump
        // allocator for user mode.
        //
        // It doesn't do a lot of mmap things at all.

        if (addr != NULL) {
                RETURN_ERROR(-9);
        }

        if (!(flags & MAP_ANONYMOUS)) {
                RETURN_ERROR(-10);
        }

        uintptr_t new_alloc = mmap_base;

        vmm_create_unbacked_range(mmap_base, len,
                                  PAGE_WRITEABLE | PAGE_USERMODE);

        mmap_base += round_up(len, 0x1000);

        RETURN_VALUE(new_alloc);
}

struct syscall_ret sys_munmap(void *addr, size_t length) {
        // nop, TODO
        RETURN_VALUE(0);
}
