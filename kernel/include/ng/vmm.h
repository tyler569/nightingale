
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
        phys_addr_t pgtable_root;

        list map_objects;
};

struct vm_map_object {
        list_node node;
        struct vm_object *object;
};

struct vm_object {
        virt_addr_t base;
        virt_addr_t top;
        int pages;

        enum vm_flags flags;
        atomic_t refcnt;

        // protection
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

// struct vm_map_object *vm_mo_split(struct vm_map_object *mo, int pages);
// struct vm_map_object *vm_mo_mid(struct vm_map_object *mo, virt_addr_t base, int pages);
// void vm_mo_merge(struct vm_map_object *mo1, struct vm_map_object *mo2);

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

void vm_user_unmap(struct vm_map_object *vmo); // ?

// struct vm_map_object *vm_user_fork_copy(struct vm_map_object *mo) 
// void vm_user_exit_unmap(struct vm_map *map);

struct vm_map *vm_user_fork(struct vm_map *old);
void vm_user_exec(struct vm_map *map);
void vm_user_exit(struct vm_map *map);

#endif // NG_VMM_H

