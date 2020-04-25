
#include <basic.h>
// #include <sys/types.h>

typedef uintptr_t virt_addr_t; // -> sys/types.h ?

enum vm_state {
        VM_INVALID = 0,
        VM_FREE = 1,
        VM_INUSE = 2,

        VM_EXCL      = (1 << 5),  // only in one memory space
        VM_CLOEXEC   = (1 << 6),  // process itself
        VM_SHARED    = (1 << 7),  // shared witeable
        VM_COW       = (1 << 8),  // shared copy-on-write
};

struct vm_map;
struct vm_map_object;
struct vm_object;

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



/*
 * New virtual memory maps are craeted with one object spanning all available
 * address space.
 *
 * You can then split that on the low order (give me the lowest available
 *   100 pages)
 * or carve a section out of the middle (I need 100 pages at 0x10000000)
 *
 */


/*
 * Internal things:
vm_mo_split(vm_map_object, pages)
        assert(object->state == VM_FREE | VM_EXCL)

vm_mo_mid(vm_map_object, virt_addr_t, pages) -> middle
        // all 3 end up on the list though!
*/

/*
 * illustrative example:
task_load(Elf *elf) ->
        vm_user_cloexec()
        x = vm_mo_mid(USER_EXE_BASE, elf->len)
        vm_mo_set(x, VM_CLOEXEC)
        copy_to_userspace(x, elf->data, elf->len);
*/

/*
 * vision synopsis
vm_kernel_init  -> init this in the kernel (based on the pm reservations ?)
vm_alloc        -> allocate space in the kernel vm space
vm_free         -> free space in the kernel vm space

vm_user_new
vm_user_fork
vm_user_mmap
vm_user_cloexec -> closes CLOEXEC mappings and decrefs them
vm_user_munmap  -> closes that mapping and decrefs it
vm_user_exit    -> closes all mappings and decrefs them
*/
 






