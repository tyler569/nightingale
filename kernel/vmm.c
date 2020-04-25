
#include <basic.h>
// #include <sys/types.h>

typedef uintptr_t virt_addr_t; // -> sys/types.h ?

enum vm_state {
        VM_INVALID,
        VM_FREE,
        VM_INUSE,

        VM_EXCL,      // only in one memory space
        VM_CLOEXEC,   // process itself
        VM_SHARED,    // shared witeable
        VM_COW,       // shared copy-on-write
};


// process 1 - 1 vm_map > - < vm_object (rc)


/* 
 * Things of note:
 * COW copies the WHOLE object! -- CREATES A NEW ONE
 *
 * a vm_object is the 1:1 exclusive owner of any physical pages involved.
 * they may be mapped un mutliple memory spaces, but they are ALL the one
 * vm_object. This means the vm_object refcnt governs the lifetime of
 * physical pages.
 *
 *
 * GOALS:
 *
 * This system should be able to support both a kernel map and to allocate
 * space for things in the kernel and also seperate userspace maps and to
 * allocate space for things like mmaps.
 *
 */

struct vm_map {
        phys_addr_t page_table_base; // ? -- what about the kernel one?

        list map_objects;
};

/*
 * Should the vm_map start with a single big unmapped vm_object spanning all
 * space it is allowed to allocate?
 *
 * I believe the vm_map's object list should be sorted.
 *
 */


struct vm_map_object {
        list_node node;
        struct vm_object *object;
};

struct vm_object {
        virt_addr_t base;
        virt_addr_t top;

        enum vm_state state;
        atomic_t refcnt;

        // protection
};
