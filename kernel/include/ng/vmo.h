#pragma once

#include <list.h>
#include <ng/fs/vnode.h>
#include <ng/sync.h>
#include <rbtree.h>
#include <stdatomic.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/cdefs.h>
#include <sys/types.h>

enum vmo_type {
	VMO_ANON,
	VMO_FILE,
	VMO_PHYS,
	VMO_ZERO,
};

struct process;

struct vmo {
	enum vmo_type type;
	size_t size;
	atomic_int refcount;
	spinlock_t lock;
	struct rbtree pages;

	struct vnode *vnode;
	off_t file_off;

	phys_addr_t phys_base;
};

struct vm_area {
	uintptr_t base;
	uintptr_t top;
	int prot;
	int flags;
	size_t vmo_off;
	struct vmo *vmo;
	struct rbnode node;
};

BEGIN_DECLS

struct vmo *vmo_new_anon(size_t size);
struct vmo *vmo_new_file(struct vnode *vnode, off_t off, size_t size);
struct vmo *vmo_new_phys(phys_addr_t base, size_t size);
struct vmo *vmo_new_zero(size_t size);
void vmo_ref(struct vmo *vmo);
void vmo_unref(struct vmo *vmo);

int vmo_get_page(struct vmo *vmo, size_t page_idx, phys_addr_t *out);
int vmo_fill_page(struct vmo *vmo, size_t page_idx, phys_addr_t *out);

int vma_compare(void *a, void *b);
void *vma_get_key(struct rbnode *node);

struct vm_area *vma_find(struct process *p, uintptr_t addr);
int vma_map(struct process *p, uintptr_t base, size_t len, int prot, int flags,
	struct vmo *vmo, size_t vmo_off);
void vma_unmap_all(struct process *p);
void vma_drop_list(struct process *p);
void vma_clone_list(struct process *dst, struct process *src);
uintptr_t vma_find_gap(
	struct process *p, size_t len, uintptr_t start, uintptr_t end);

END_DECLS
