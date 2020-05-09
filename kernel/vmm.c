
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
 * vision synopsis
vm_kernel_init  -> init this in the kernel (based on the pm reservations ?)
vm_alloc        -> allocate space in the kernel vm space
vm_free         -> free space in the kernel vm space

vm_user_new
vm_user_fork
  - note that fork should copy the vm_objects for everything that isn't
    SHARED -- including free regions.
vm_user_mmap
*/

/*
 * Noting that this is not meant to replace page table juggling -- This is
 * additional bookkeeping maintained in parallel to the page tables.
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
        struct vm_object *o;
        printf("vm map dump:\n");
        list_foreach(&map->objects, o, node) {
                vm_object_dump(o);
        }
}


struct vm_object *vm_object_split(struct vm_object *vm, size_t pages) {
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

        _list_prepend(&vm->node, &new->node);
        return new;
}

struct vm_object *vm_object_mid(struct vm_object *vm, virt_addr_t base, virt_addr_t top) {
        size_t pages_left = (base - vm->base) / PAGE_SIZE;
        size_t pages_right = (vm->top - top) / PAGE_SIZE;

        size_t pages = PAGECOUNT(base, top);

        printf(" (mid: %# 10x / %# 10x / %# 10x)\n", pages_left, pages, pages_right);

        struct vm_object *mid;

        if (pages_left > 0) {
                mid = vm_object_split(vm, pages_left);
        } else {
                mid = vm;
        }

        if (pages_right > 0)  vm_object_split(mid, pages);
        return mid;
}

void vm_object_merge(struct vm_object *mo1, struct vm_object *mo2) {
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
struct vm_object *vm_object_with(struct vm_map *map, virt_addr_t base, virt_addr_t top) {
        struct vm_object *vm;
        bool found = false;
        list_foreach(&map->objects, vm, node) {
                if (vm_object_overlaps(vm, base, top)) {
                        goto found;
                }
        }
        return NULL;
found:
        return vm;
}

// This is hacky AF
struct vm_object *vm_with(virt_addr_t addr) {
        if (addr > VMM_VIRTUAL_OFFSET) {
                return vm_object_with(vm_kernel, addr, addr);
        }
        return vm_object_with(running_process->vm, addr, addr);
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
        list_init(&vm_kernel->objects);

        struct vm_object *all = zmalloc(sizeof(struct vm_object));

        all->base = VM_KERNEL_BASE;
        all->top = VM_KERNEL_END;
        all->pages = PAGECOUNT(all->base, all->top);
        all->flags = VM_FREE;
        all->refcnt = 0;
        
        all->object = all;
        _list_append(&vm_kernel->objects, &all->node);
        
        phys_addr_t new_pgtable_root = pm_alloc_page();
        vm_kernel->pgtable_root = new_pgtable_root;
        printf("new kernel pagetables root at %# 18lx\n", new_pgtable_root);
        vmm_set_fork_base_kernel(new_pgtable_root);

        for (; mappings->base; mappings++) {
                virt_addr_t base = round_down(mappings->base, PAGE_SIZE);
                size_t len = round_up(mappings->len, PAGE_SIZE);
                printf("(trying) to map %p: %p with flags %x\n", base, len, mappings->flags);
                vmm_fork_map_range(base, p(base), len, mappings->flags);
        }

        vmm_clear_fork_base();
        vmm_set_pgtable(vm_kernel->pgtable_root);

        return vm_kernel;
}

#undef v
#undef p

virt_addr_t vm_alloc(size_t length) {
        size_t pages = round_up(length, PAGE_SIZE) / PAGE_SIZE;

        struct vm_object *vm;
        bool found_space = false;
        list_foreach(&vm_kernel->objects, vm, node) {
                if (vm->pages >= pages &&
                    vmo_is_free(vm)) {
                        found_space = true;
                        break;
                }
        }

        if (!found_space) {
                // impossible to fulfill request
                return VM_NULL;
        }

        if (vm->pages > pages) {
                vm_object_split(vm, pages);
        }

        printf("vm_alloc() -> ");
        vm_object_dump(vm);
        vmm_create_unbacked_range(vm->base, vm->pages * PAGE_SIZE, PAGE_WRITEABLE);

        vm->flags = VM_INUSE;
        vm->refcnt = 1;
        return vm->base;
}

virt_addr_t vmm_reserve(size_t length) {
        return vm_alloc(length);
}

void vm_free(virt_addr_t addr) { //, size_t pages) { ?
        struct vm_object *vm;
        bool found = false;
        list_foreach(&vm_kernel->objects, vm, node) {
                if (vm->base == addr) {
                        found = true;
                        break;
                }
        }

        if (!found) {
                printf("vm_free: non-allocated address!\n");
                return;
        }

        vmm_unmap_range_free(vm->base, vm->pages * PAGE_SIZE);

        vm->flags = VM_FREE;
}


// USER map functions

struct vm_map *vm_user_new() {
        struct vm_map *map = zmalloc(sizeof(struct vm_map));
        list_init(&map->objects);

        struct vm_object *all = zmalloc(sizeof(struct vm_object));

        all->base = VM_USER_BASE;
        all->top = VM_USER_END;
        all->pages = PAGECOUNT(all->base, all->top);
        all->flags = VM_FREE;
        all->refcnt = 0;
        
        _list_append(&map->objects, &all->node);
        return map;
}

virt_addr_t vm_user_reserve(struct vm_map *map,
                virt_addr_t base, virt_addr_t top, enum vm_flags flags) {
        printf("vm_user_reserve before: \n");
        vm_map_dump(map);
        struct vm_object *vm;

        vm = vm_object_with(map, base, top);

        size_t pages = PAGECOUNT(base, top);

        if (vm->pages > pages) {
                vm = vm_object_mid(vm, base, top);
        }

        vm->object->flags = VM_INUSE | flags;

        // map range? nah

        printf("vm_user_alloc after: \n");
        vm_map_dump(map);

        return vm->object->base;
}

virt_addr_t vm_user_alloc(struct vm_map *map, size_t len, enum vm_flags flags) {
        struct vm_object *vm;
        list_foreach(map->objects, vm, node) {
                if (vm->pages >= PAGECOUNT(LEN)) {
                        // pull some space for this alloc
                        break;
                }
        }
        return vm->base; // or something
}

struct vm_object *vm_user_fork_copy(struct vm_object *vm) {
        struct vm_object *new = zmalloc(sizeof(struct vm_object));
        new->base = vm->base;
        new->top = vm->top;
        new->pages = vm->pages;
        new->flags = vm->flags;

        // copy the mappings ? or back to vmm_fork ?

        return new;
}

struct vm_map *vm_user_fork(struct vm_map *old) {
        struct vm_map *new_map = zmalloc(sizeof(struct vm_map));
        list_init(&new_map->objects);

        // should this make the new page table and copy mappings?
        // probably I think

        struct vm_object *vm;
        list_foreach(&old->objects, vm, node) {
                struct vm_object *new = vm_user_fork_copy(vm);
                _list_append(&new_map->objects, &new->node);
        }

        return new;
}


