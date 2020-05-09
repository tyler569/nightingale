
#pragma once
#ifndef NG_VMM_H
#define NG_VMM_H
#include <basic.h>
#include <ng/mutex.h>
#include <ng/types.h>
#include <nc/list.h>

#if X86_64
#include <ng/x86/64/vmm.h>
#elif I686
#include <ng/x86/32/vmm.h>
#endif

#define VM_NULL (virt_addr_t)(-1)

enum vm_flags {
        VM_INVALID = 0,

        VM_FREE      = (1 << 0),
        VM_INUSE     = (1 << 1),

        // MAP_PRIVATE:
        VM_COW       = (1 << 7),  // shared copy-on-write
        // MAP_SHARED:
        VM_SHARED    = (1 << 8),  // shared writeable

        VM_EAGERCOPY = (1 << 9),  // not shared at all, not implemented.
};

struct vm_map;
struct vm_object;

struct vm_map {
        phys_addr_t pgtable_root;

        list objects;
};

struct vm_object {
        list_node node;

        virt_addr_t base;
        virt_addr_t top;
        size_t pages;

        enum vm_flags flags;
};

// just used in vm_kernel_init
struct kernel_mappings {
        virt_addr_t base;
        size_t len;
        int flags;
};


#define vmo_is_free(vmo)    (((vmo)->flags & VM_FREE) != 0)
#define vmo_is_shared(vmo)  (((vmo)->flags & VM_SHARED) != 0)
#define vmo_is_private(vmo) (((vmo)->flags & VM_COW) != 0)
#define vmo_is_cow(vmo)     vmo_is_private(vmo)

#if X86_64
#define VM_USER_BASE 0x1000
#define VM_USER_END 0x7FFFFFFF0000
#elif I686
#define VM_USER_BASE 0x1000
#define VM_USER_END 0xBFFF0000
#endif

#define PAGECOUNT(base, top) (round_up(top - base, PAGE_SIZE) / PAGE_SIZE)

// struct vm_object *vm_object_split(struct vm_object *mo, size_t pages);
// struct vm_object *vm_object_mid(struct vm_object *mo, virt_addr_t base, size_t pages);
// void vm_object_merge(struct vm_object *mo1, struct vm_object *mo2);

// KERNEL map functions

extern struct vm_map *vm_kernel;
extern mutex_t fork_mutex;

struct vm_map *vm_kernel_init();

virt_addr_t vm_alloc(size_t length);
void vm_free(virt_addr_t addr);

// USER map functions

struct vm_map *vm_user_new();

virt_addr_t vm_user_alloc(struct vm_map *map,
                virt_addr_t base, virt_addr_t top, enum vm_flags flags) ;

void vm_user_unmap(struct vm_object *vmo); // ?
void vm_user_remap(struct vm_object *vmo);

// struct vm_object *vm_user_fork_copy(struct vm_object *mo) 
// void vm_user_exit_unmap(struct vm_map *map);

struct vm_map *vm_user_fork(struct vm_map *old);
void vm_user_exec(struct vm_map *map);
void vm_user_exit(struct vm_map *map);

struct vm_object *vm_with(virt_addr_t addr);

void vm_map_dump(struct vm_map *map);

#endif // NG_VMM_H

