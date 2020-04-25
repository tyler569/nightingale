
#include <basic.h>
// #include <sys/types.h>

typedef uintptr_t virt_addr_t; // -> sys/types.h ?

enum vm_flags {
        VM_INVALID = 0,

        VM_FREE      = (1 << 0),
        VM_INUSE     = (1 << 1),

        // MAP_PRIVATE:
        VM_COW       = (1 << 7),  // shared copy-on-write (refcnt significant)
        // MAP_SHARED:
        VM_SHARED    = (1 << 8),  // shared writeable     (refcnt significant)

        VM_EAGERCOPY = (1 << 9),  // not shared at all, not implemented.
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

struct vm_map_object {
        list_node node;
        struct vm_object *object;
};

struct vm_object {
        virt_addr_t base;
        virt_addr_t top;

        enum vm_flags flags;
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
        assert(object->flags == VM_FREE)

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
  - note that fork should copy the vm_objects for everything that isn't
    SHARED -- including free regions.
vm_user_mmap
vm_user_cloexec -> closes CLOEXEC mappings and decrefs them
vm_user_munmap  -> closes that mapping and decrefs it
vm_user_exit    -> closes all mappings and decrefs them
*/
 

/*
 * Noting that this is not meant to replace page table juggling -- This is
 * additional bookkeeping maintained in parallel to the page tables.
 *
 * This will need to be consulted in vmm_fork though, to get the right
 * things copied and not copied.
 *
 */

#define vmo_is_free(vmo)    ((vmo)->flags & VM_FREE != 0)
#define vmo_is_shared(vmo)  ((vmo)->flags & VM_SHARED != 0)
#define vmo_is_private(vmo) ((vmo)->flags & VM_COW != 0)
#define vmo_is_cow(vmo)     vmo_is_private(vmo)

struct vm_map_object *vm_mo_split(struct vm_map_object *mo, int pages) {
        struct vm_object *vm = mo->object;
        assert(vm->flags == VM_FREE);
        assert(vm->refcnt == 0);

        struct vm_object *new = zmalloc(sizeof(struct vm_object));
        new->base = vm->base + pages * PAGE_SIZE;
        new->top = vm->top;
        new->pages = vm->pages - pages;

        vm->top = new->base;
        vm->pages = pages;

        new->flags = vm->flags;
        new->refcnt = 0;

        struct vm_map_object *new_mo = zmalloc(sizeof(struct vm_map_object));
        new_mo->object = new;
        _list_append(&mo->node, &new_mo->node);
        return new_mo;
}

struct vm_map_object *vm_mo_mid(struct vm_map_object *mo, virt_addr_t base, int pages) {
        struct vm_object *vm = mo->object;
        int pages_left = (base - vm->base) / PAGE_SIZE;
        assert(pages_left >= 0);
        int pages_right = (vm->top - (base + pages * PAGE_SIZE)) / PAGE_SIZE;
        assert(pages_right >= 0);

        struct vm_map_object *right = vm_mo_split(mo, pages_left);
        struct vm_map_object *mid = vm_mo_split(right, pages);

        return mid;
}


// KERNEL map functions

struct vm_map *vm_kernel;
#define VM_NULL (virt_addr_t)-1

struct vm_map vm_kernel_init() {
        // TODO -- earlyheap? where does this memory come from?
        // maybe I should do the thing where the heap can expand now...
}

virt_addr_t vm_alloc(int pages) {
        struct vm_map_object *mo;
        bool found_space = false;
        list_foreach_safe(&vm_kernel->map_objects, mo, node) {
                if (mo->object->pages >= pages &&
                    vmo_is_free(mo->object)) {
                        found_space = true;
                        break;
                }
        }

        if (!found_space) {
                // impossible to fulfill request
                return VM_NULL;
        }

        if (mo->object->pages > pages) {
                vm_mo_split(mo, pages);
        }
        mo->object->flags = VM_INUSE;
        mo->object->refcnt = 1;
        return mo->object->base;
}

void vm_free(virt_addr_t addr) { //, int pages) { ?
        struct vm_map_object *mo;
        bool found = false;
        list_foreach_safe(&vm_kernel->map_objects, mo, node) {
                if (mo->object->base == addr) {
                        found = true;
                        break;
                }
        }

        if (!found) {
                printf("vm_free: non-allocated address!\n");
                return;
        }

        mo->object->flags = VM_FREE;
}



// USER map functions

#if X86_64
#define VM_USER_BASE 0x1000
#define VM_USER_END 0x7FFFFFFF0000
#elif I686
#define VM_USER_BASE 0x1000
#define VM_USER_END 0xBFFF0000
#endif

#define PAGECOUNT(base, top) (round_up(top - base, PAGE_SIZE) / PAGE_SIZE)

struct vm_map *vm_user_new() {
        struct vm_map *map = zmalloc(sizeof(struct vm_map));
        list_init(&map->map_objects);

        struct vm_map_object *mo_all = zmalloc(sizeof(struct vm_map_object));
        struct vm_object *all = zmalloc(sizeof(struct vm_object));

        all->base = VM_USER_BASE;
        all->top = VM_USER_END;
        all->pages = PAGECOUNT(all->base, all->top);
        all->flags = VM_FREE;
        all->refcnt = 1;
        
        mo_all->object = all;
        _list_append(&map->map_objects, &mo_all->node);
}

virt_addr_t vm_user_alloc(struct vm_map *map,
                virt_addr_t base, virt_addr_t top, enum vm_flags flags) {
        struct vm_map_object *mo;
        bool found_space = false;
        int pages = PAGECOUNT(base, top);
        list_foreach(&map->map_objects, mo, node) {
                if (mo->object->pages >= pages && vmo_is_free(mo)) {
                        found_space = true;
                        break;
                }
        }

        if (!found_space) {
                printf("vm_user_alloc: impossible to service request\n");
                return NULL;
        }

        if (mo->object->pages > pages) {
                vm_object_split(mo);
        }

        mo->object->flags = VM_INUSE | flags;
        if (!vmo_is_private(mo->object)) {
                mo->refcount = 1;
        }
        return mo->object->top;
}

struct vm_map_object *vm_user_fork_copy(struct vm_map_object *mo) {
        struct vm_map_object *new = zmalloc(sizeof(struct vm_map_object));

        new->object = mo->object;
        new->object->refcnt++;

        if (vmo_is_cow(mo)) {
                vmm_remap(mo->object->base, mo->object->base, VM_COW);
        }
        
        vmm_fork_map(new->object->base, new->object->top, new->object->flags);
}

static mutex_t fork_mutex = KMUTEX_INIT;

struct vm_map *vm_user_fork(struct vm_map *old) {
        struct vm_map *new = zmalloc(sizeof(struct vm_map));
        list_init(&new->map_objects);

        mutex_await(&fork_mutex);

        new->page_table_base = pm_alloc_page();
        vmm_set_fork_base(new->page_table_base);

        struct vm_map_object *mo;
        list_foreach(&old->map_objects, mo, node) {
                struct vm_map_object *new_mo = vm_user_fork_copy(mo);
                _list_append(&new->map_objects, &new_mo->node);
        }

        mutex_unlock(&fork_mutex);
}

void vm_user_unmap(struct vm_map_object *vmo) {
        if (vmo_is_free(vmo->object)) {
                return;
        }

        vmo->object->refcnt--;
        if (vmo->object->refcnt == 0) {
                // dealloc pages or something
        } else {
                // need to keep the old object around, it's just not in this
                // address space anymore.

                struct vm_object new_obj = zmalloc(sizeof(struct vm_object));
                new_obj->base = vmo->object->base;
                new_obj->top = vmo->object->top;
                new_obj->pages = vmo->object->pages;
                new_obj->refcnt = 0;

                vmo->object = new_obj;
        }
}

void vm_mo_merge(struct vm_map_object *mo1, struct vm_map_object *mo2) {S
        assert(mo1->object->flags == VM_FREE);
        assert(mo2->object->flags == VM_FREE);

        assert(mo1->object->top == mo2->object->base);

        mo1->object->pages += mo2->object->pages;
        mo1->object->top = mo2->object->top;

        free(mo2->object);
}

void vm_user_exit_unmap(struct vm_map *map) {
        struct vm_map_object *mo, *tmp;
        struct vm_map_object *first =
                list_head_entry(struct vm_map_object, &map->map_objects, node);
        list_foreach_safe(&map->vm_objects, mo, tmp, node) {
                vm_user_unmap(mo);
                if (mo != first)
                        vm_mo_merge(first, mo);
        }
}

void vm_user_exec(struct vm_map *map) {
        // exec unmapps everything.
        vm_user_exit_unmap(map);
}

void vm_user_exit(struct vm_map *map) {
        assert(map);
        vm_user_exit_unmap(map);
        struct vm_map_object *mo =
                list_head_entry(struct vm_map_object, &map->map_objects, node);
        assert(mo);
        assert(mo->object);
        assert(mo->object->flags == VM_FREE);
        free(mo->object);
        free(mo);
        free(map);
}


/* LATER MAYBE
bool vm_object_overlaps(struct vm_object *obj, virt_addr_t base, virt_addr_t top) {
        return (obj->base < top && base < obj->top);
}

void vm_object_drop_overlap(struct vm_map_object *mo, virt_addr_t base, virt_addr_t top) {
        if (overlap is complete) {
                mo->flags = VM_FREE;
                vmm_unmap
        }

        left, right, split, split
        unmap the overlap
}

void vm_user_coalesce(struct vm_map *map) {
        // merge adjacent free sections
}

void vm_user_raw_del(struct vm_map *map, virt_addr_t base, size_t len) {
        struct vm_map_object *mo, *tmp;
        list_foreach_safe(&map->map_objects, mo, tmp, node) {
                if (vm_object_overlaps(mo->object, base, base + len)) {
                        vm_object_drop_overlap(mo, base, base + len);
                }
        }
        vm_map_coalesce(map);
}
*/






