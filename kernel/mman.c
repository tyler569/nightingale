
// #define DEBUG
#include <ng/basic.h>
#include <ng/debug.h>
#include <ng/mman.h>
#include <ng/print.h>
#include <ng/syscall.h>
#include <ng/thread.h>
#include <ng/vmm.h>
#include <arch/memmap.h>
#include <ng/fs.h>

// drivers and modules should call this if they want a large amount of virtual
// space available for use over time.
//
// ex: malloc reserves 128MB for it to manage.
//
// there's no way to free currently.

char *kernel_reservable_vma = (char *)KERNEL_RESERVABLE_SPACE;

#if X86_64
// on X64, there are some set of things that need to be in the top 2G
// the current use-case for this are kernel modules compiled mcmodel=kernel
// which must be located above -2G
//
// TODO: mode this base to the actual "above initfs" address
char *kernel_high_reservable_vma = (char *)0xFFFFFFFFC0000000;
#endif

void *vmm_reserve(size_t len) {
        len = round_up(len, 0x1000);

        void *res = kernel_reservable_vma;
        // printf("RESERVING RANGE %p + %lx\n", res, len);
        kernel_reservable_vma += len;

        vmm_create_unbacked_range((uintptr_t)res, len, PAGE_WRITEABLE);
        return res;
}

void *high_vmm_reserve(size_t len) {
#if I686
        return vmm_reserve(len);
#elif X86_64
        len = round_up(len, 0x1000);

        void *res = kernel_high_reservable_vma;
        kernel_high_reservable_vma += len;

        vmm_create_unbacked_range((uintptr_t)res, len, PAGE_WRITEABLE);
        return res;
#endif
}

struct syscall_ret sys_mmap(void *addr, size_t len, int prot, int flags, int fd,
                            off_t offset) {
        len = round_up(len, 0x1000);

        // TODO:
        // This is a very dumb simple bump allocator just being made
        // to make it possible for me to make a dumb simple bump
        // allocator for user mode.
        //
        // It doesn't do a lot of mmap things at all.

        if (addr != NULL) {
                RETURN_ERROR(ETODO);
        }

        if (!(flags & MAP_ANONYMOUS)) {
                RETURN_ERROR(ETODO);
        }

        uintptr_t new_alloc = running_process->mmap_base;

        vmm_create_unbacked_range(new_alloc, len,
                                  PAGE_WRITEABLE | PAGE_USERMODE);

        running_process->mmap_base += len;

        RETURN_VALUE(new_alloc);
}

struct syscall_ret sys_munmap(void *addr, size_t length) {
        // nop, TODO
        RETURN_VALUE(0);
}
