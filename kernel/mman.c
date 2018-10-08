
// #define DEBUG
#include "mman.h"
#include <basic.h>
#include <debug.h>
#include <stdint.h>
#include <stddef.h>
#include <malloc.h>
#include <print.h>
#include <debug.h>
#include <vmm.h>
#include <fs/vfs.h> // for off_t, bad placememt?
#include <syscall.h>

#if X86_64
uintptr_t mmap_base = 0x1100000000;
#elif I686
uintptr_t mmap_base = 0x50000000;
#endif

struct syscall_ret sys_mmap(
        void* addr, size_t len, int prot, int flags, int fd, off_t offset) {

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
    
    vmm_create_unbacked_range(mmap_base, len, PAGE_WRITEABLE | PAGE_USERMODE);

    mmap_base += round_up(len, 0x1000);

    RETURN_VALUE(new_alloc);
}

struct syscall_ret sys_munmap(void* addr, size_t length) {
    // nop, TODO
    RETURN_VALUE(0);
}

