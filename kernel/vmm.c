
#include <basic.h>
#include <ng/thread.h>
#include <ng/mutex.h>
#include <ng/pmm.h>
#include <ng/vmm.h>
#include <nc/list.h>
#include <nc/stdio.h>
#include <nc/stdlib.h>

struct vm_map *vm_kernel = NULL;
mutex_t fork_mutex = KMUTEX_INIT;

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
vm__kernel_init  -> init this in the kernel (based on the pm reservations ?)
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


// INTERNAL things

#define FLAG_PRINT(field, flag) do { \
        if (field & flag) { \
                if (first) { \
                        first = false; \
                } else { \
                        printf("|"); \
                } \
                printf(#flag); \
        } \
} while(0)

void vm_object_dump(struct vm_object *o) {
        printf("vm_object{.base=%p, .top=%p, .pages=%zu, .flags=",
                o->base, o->top, o->pages);

        bool first = true;

        FLAG_PRINT(o->flags, VM_FREE);
        FLAG_PRINT(o->flags, VM_INUSE);
        FLAG_PRINT(o->flags, VM_COW);
        FLAG_PRINT(o->flags, VM_SHARED);
        FLAG_PRINT(o->flags, VM_EAGERCOPY);
        printf(", .refcnt=%i}\n", o->refcnt);
}

#undef FLAG_PRINT

void vm_map_dump(struct vm_map *map) {
        struct vm_map_object *mo;
        struct vm_object *o;
        printf("vm map dump:\n");
        list_foreach(&map->map_objects, mo, node) {
                vm_object_dump(mo->object);
        }
}


struct vm_map_object *vm_mo_split(struct vm_map_object *mo, size_t pages) {
        struct vm_object *vm = mo->object;

        /*
        printf("splitting %x ", vm->flags);
        printf("%# 18lx ", vm->base);
        printf("%# 18lx ", vm->top);
        printf(": %5i ", pages);
        printf("/ (%# 10x)", pages * PAGE_SIZE);
        printf("\n");
        */

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
        _list_prepend(&mo->node, &new_mo->node);
        return new_mo;
}

struct vm_map_object *vm_mo_mid(struct vm_map_object *mo,
                virt_addr_t base, virt_addr_t top) {
        struct vm_object *vm = mo->object;
        size_t pages_left = (base - vm->base) / PAGE_SIZE;
        size_t pages_right = (vm->top - top) / PAGE_SIZE;

        size_t pages = PAGECOUNT(base, top);

        printf(" (mid: %# 10x / %# 10x / %# 10x)\n", pages_left, pages, pages_right);

        struct vm_map_object *mid;

        if (pages_left > 0) {
                mid = vm_mo_split(mo, pages_left);
        } else {
                mid = mo;
        }

        if (pages_right > 0)  vm_mo_split(mid, pages);
        return mid;
}

void vm_mo_merge(struct vm_map_object *mo1, struct vm_map_object *mo2) {
        assert(mo1->object->flags == VM_FREE);
        assert(mo2->object->flags == VM_FREE);

        assert(mo1->object->top == mo2->object->base);

        mo1->object->pages += mo2->object->pages;
        mo1->object->top = mo2->object->top;

        free(mo2->object);
}

bool vm_object_overlaps(struct vm_object *o, virt_addr_t base, virt_addr_t top) {
        if (base == top) {
                return (o->base <= base && base < o->top);
        }

        return (o->base < top && base < o->top);
}

/*
 * Find the map_object containing the specified range
 */
struct vm_map_object *vm_mo_with(struct vm_map *map,
                virt_addr_t base, virt_addr_t top) {
        struct vm_map_object *mo;
        bool found = false;
        list_foreach(&map->map_objects, mo, node) {
                if (vm_object_overlaps(mo->object, base, top)) {
                        goto found;
                }
        }
        return NULL;
found:
        return mo;
}

// This is hacky AF
struct vm_map_object *vm_with(virt_addr_t addr) {
        if (addr > VMM_VIRTUAL_OFFSET) {
                return vm_mo_with(vm_kernel, addr, addr);
        }
        return vm_mo_with(running_process->vm, addr, addr);
}

// KERNEL map functions

#if X86_64
#define VM_KERNEL_BASE (VMM_VIRTUAL_OFFSET + 0x1000000)
#define VM_KERNEL_END  0xFFFFFFFFFFFFF000
#elif I686
#define VM_KERNEL_BASE (VMM_VIRTUAL_OFFSET + 0x1000000)
#define VM_KERNEL_END  0xF0000000
#endif

#define v(p) (p + VMM_VIRTUAL_OFFSET)
#define p(v) (v - VMM_VIRTUAL_OFFSET)

struct vm_map *vm_kernel_init(struct kernel_mappings *mappings) {
        vm_kernel = zmalloc(sizeof(*vm_kernel));
        list_init(&vm_kernel->map_objects);

        struct vm_map_object *mo_all = zmalloc(sizeof(struct vm_map_object));
        struct vm_object *all = zmalloc(sizeof(struct vm_object));

        all->base = VM_KERNEL_BASE;
        all->top = VM_KERNEL_END;
        all->pages = PAGECOUNT(all->base, all->top);
        all->flags = VM_FREE;
        all->refcnt = 0;
        
        mo_all->object = all;
        _list_append(&vm_kernel->map_objects, &mo_all->node);
        
        phys_addr_t new_pgtable_root = pm_alloc_page();
        vm_kernel->pgtable_root = new_pgtable_root;
        printf("new kernel pagetables root at %# 18lx\n", new_pgtable_root);
        vmm_set_fork_base_kernel(new_pgtable_root);

        for (; mappings->base; mappings++) {
                virt_addr_t base = round_down(mappings->base, PAGE_SIZE);
                size_t len = round_up(mappings->len, PAGE_SIZE);
                printf("(trying) to map %p: %p with flags %x\n", base, len, mappings->flags);
                vmm_fork_map_range(base, p(base), len, mappings->flags);
                //vmm_fork_copyfrom(new->object->base, mo->object->base, mo->object->pages);
        }

        vmm_clear_fork_base();
        vmm_set_pgtable(vm_kernel->pgtable_root);

        return vm_kernel;
}

#undef v
#undef p

virt_addr_t vm_alloc(size_t length) {
        size_t pages = round_up(length, PAGE_SIZE) / PAGE_SIZE;

        struct vm_map_object *mo;
        bool found_space = false;
        list_foreach(&vm_kernel->map_objects, mo, node) {
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

        printf("vm_alloc() -> ");
        vm_object_dump(mo->object);
        vmm_create_unbacked_range(mo->object->base, mo->object->pages * PAGE_SIZE, PAGE_WRITEABLE);

        mo->object->flags = VM_INUSE;
        mo->object->refcnt = 1;
        return mo->object->base;
}

virt_addr_t vmm_reserve(size_t length) {
        return vm_alloc(length);
}

void vm_free(virt_addr_t addr) { //, size_t pages) { ?
        struct vm_map_object *mo;
        bool found = false;
        list_foreach(&vm_kernel->map_objects, mo, node) {
                if (mo->object->base == addr) {
                        found = true;
                        break;
                }
        }

        if (!found) {
                printf("vm_free: non-allocated address!\n");
                return;
        }

        vmm_unmap_range_free(mo->object->base, mo->object->pages * PAGE_SIZE);

        mo->object->flags = VM_FREE;
}


// USER map functions

struct vm_map *vm_user_new() {
        struct vm_map *map = zmalloc(sizeof(struct vm_map));
        list_init(&map->map_objects);

        struct vm_map_object *mo_all = zmalloc(sizeof(struct vm_map_object));
        struct vm_object *all = zmalloc(sizeof(struct vm_object));

        all->base = VM_USER_BASE;
        all->top = VM_USER_END;
        all->pages = PAGECOUNT(all->base, all->top);
        all->flags = VM_FREE;
        all->refcnt = 0;
        
        mo_all->object = all;
        _list_append(&map->map_objects, &mo_all->node);
        return map;
}

virt_addr_t vm_user_alloc(struct vm_map *map,
                virt_addr_t base, virt_addr_t top, enum vm_flags flags) {
        printf("vm_user_alloc before: \n");
        vm_map_dump(map);
        struct vm_map_object *mo;

        mo = vm_mo_with(map, base, top);

        size_t pages = PAGECOUNT(base, top);

        if (mo->object->pages > pages) {
                mo = vm_mo_mid(mo, base, top);
        }

        mo->object->flags = VM_INUSE | flags;
        if (!vmo_is_private(mo->object)) {
                mo->object->refcnt = 1;
        }

        if (map->pgtable_root == running_process->vm->pgtable_root) {
                vmm_create_unbacked_range(base, top - base, PAGE_USERMODE | PAGE_WRITEABLE);
        } else {
                assert(0); // TODO mapping into someone else's address space
        }

        printf("vm_user_alloc after: \n");
        vm_map_dump(map);

        return mo->object->base;
}

struct vm_map_object *vm_user_fork_copy(struct vm_map_object *mo) {
        struct vm_map_object *new = zmalloc(sizeof(struct vm_map_object));

        new->object = mo->object;
        new->object->refcnt++;

        if (vmo_is_cow(mo->object)) {
                vmm_remap(mo->object->base, mo->object->top, VM_COW);
        }
        
        vmm_fork_copyfrom(new->object->base, mo->object->base, mo->object->pages);
        return new;
}

struct vm_map *vm_user_fork(struct vm_map *old) {
        struct vm_map *new = zmalloc(sizeof(struct vm_map));
        list_init(&new->map_objects);

        new->pgtable_root = pm_alloc_page();
        vmm_set_fork_base(new->pgtable_root);

        struct vm_map_object *mo;
        list_foreach(&old->map_objects, mo, node) {
                struct vm_map_object *new_mo = vm_user_fork_copy(mo);
                _list_append(&new->map_objects, &new_mo->node);
        }
        
        vmm_clear_fork_base();
        return new;
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

                struct vm_object *new_obj = zmalloc(sizeof(struct vm_object));
                new_obj->flags = vmo->object->flags;
                new_obj->base = vmo->object->base;
                new_obj->top = vmo->object->top;
                new_obj->pages = vmo->object->pages;
                new_obj->refcnt = 0;

                vmo->object = new_obj;
        }
}

/*
 * Intended to reuse memory released by vm_user_unmap. Currently used when
 * performing copy-on-write operations
 */
void vm_user_remap(struct vm_map_object *mo) {
        assert(mo->object->refcnt == 0);
        mo->object->refcnt = 1;
}

void vm_user_exit_unmap(struct vm_map *map) {
        struct vm_map_object *mo, *tmp;
        struct vm_map_object *first =
                list_head_entry(struct vm_map_object, &map->map_objects, node);
        list_foreach_safe(&map->map_objects, mo, tmp, node) {
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






