#include <ng/pmm.h>
#include <ng/thread.h>
#include <ng/vmm.h>
#include <ng/vmo.h>
#include <rbtree.h>
#include <stdlib.h>
#include <string.h>
#include <sys/cdefs.h>

struct vmo_page {
	size_t index;
	phys_addr_t phys;
	struct rbnode node;
};

static int vmo_page_compare(void *a, void *b) {
	size_t ka = *(size_t *)a;
	size_t kb = *(size_t *)b;
	if (ka < kb)
		return -1;
	if (ka > kb)
		return 1;
	return 0;
}

static void *vmo_page_key(struct rbnode *node) {
	struct vmo_page *page = container_of(struct vmo_page, node, node);
	return &page->index;
}

static struct vmo *vmo_new(enum vmo_type type, size_t size) {
	struct vmo *vmo = malloc(sizeof(*vmo));
	memset(vmo, 0, sizeof(*vmo));
	vmo->type = type;
	vmo->size = size;
	vmo->refcount = 1;
	vmo->lock = (spinlock_t) { 0 };
	vmo->pages = (struct rbtree) {
		.root = nullptr,
		.compare = vmo_page_compare,
		.get_key = vmo_page_key,
	};
	return vmo;
}

struct vmo *vmo_new_anon(size_t size) {
	return vmo_new(VMO_ANON, size);
}

struct vmo *vmo_new_file(struct vnode *vnode, off_t off, size_t size) {
	struct vmo *vmo = vmo_new(VMO_FILE, size);
	vmo->vnode = vnode;
	vmo->file_off = off;
	return vmo;
}

struct vmo *vmo_new_phys(phys_addr_t base, size_t size) {
	struct vmo *vmo = vmo_new(VMO_PHYS, size);
	vmo->phys_base = base;
	return vmo;
}

struct vmo *vmo_new_zero(size_t size) {
	return vmo_new(VMO_ZERO, size);
}

void vmo_ref(struct vmo *vmo) {
	atomic_fetch_add(&vmo->refcount, 1);
}

static void vmo_free_node(struct rbnode *node) {
	if (!node)
		return;
	vmo_free_node(node->left);
	vmo_free_node(node->right);
	struct vmo_page *page = container_of(struct vmo_page, node, node);
	free(page);
}

static void vmo_free_pages(struct vmo *vmo) {
	vmo_free_node(vmo->pages.root);
	vmo->pages.root = nullptr;
}

void vmo_unref(struct vmo *vmo) {
	if (!vmo)
		return;
	if (atomic_fetch_sub(&vmo->refcount, 1) != 1)
		return;
	vmo_free_pages(vmo);
	free(vmo);
}

static struct vmo_page *vmo_page_find(struct vmo *vmo, size_t index) {
	struct rbnode *node = rbtree_search(&vmo->pages, &index);
	if (!node)
		return nullptr;
	return container_of(struct vmo_page, node, node);
}

int vmo_get_page(struct vmo *vmo, size_t page_idx, phys_addr_t *out) {
	if (!vmo || !out)
		return -1;
	if (page_idx * PAGE_SIZE >= vmo->size)
		return -1;

	if (vmo->type == VMO_PHYS) {
		*out = vmo->phys_base + page_idx * PAGE_SIZE;
		return 0;
	}

	spin_lock(&vmo->lock);
	struct vmo_page *page = vmo_page_find(vmo, page_idx);
	if (!page) {
		spin_unlock(&vmo->lock);
		return -1;
	}
	*out = page->phys;
	spin_unlock(&vmo->lock);
	return 0;
}

int vmo_fill_page(struct vmo *vmo, size_t page_idx, phys_addr_t *out) {
	if (!vmo || !out)
		return -1;
	if (page_idx * PAGE_SIZE >= vmo->size)
		return -1;

	if (vmo->type == VMO_PHYS) {
		*out = vmo->phys_base + page_idx * PAGE_SIZE;
		return 0;
	}

	spin_lock(&vmo->lock);
	struct vmo_page *existing = vmo_page_find(vmo, page_idx);
	if (existing) {
		*out = existing->phys;
		spin_unlock(&vmo->lock);
		return 0;
	}

	phys_addr_t phy = pm_alloc();
	memset((void *)(phy + HW_MAP_BASE), 0, PAGE_SIZE);

	if (vmo->type == VMO_FILE) {
		size_t off = (size_t)vmo->file_off + page_idx * PAGE_SIZE;
		size_t file_len = vmo->vnode->len;
		if (off < file_len) {
			size_t avail = file_len - off;
			size_t to_copy = MIN((size_t)PAGE_SIZE, avail);
			memcpy((void *)(phy + HW_MAP_BASE),
				(char *)vmo->vnode->data + off, to_copy);
		}
	}

	struct vmo_page *page = malloc(sizeof(*page));
	page->index = page_idx;
	page->phys = phy;
	rbtree_insert(&vmo->pages, &page->node);
	*out = phy;
	spin_unlock(&vmo->lock);
	return 0;
}

struct vm_area *vma_find(struct process *p, uintptr_t addr) {
	if (!p)
		return nullptr;
	list_for_each (&p->vmas) {
		struct vm_area *vma = container_of(struct vm_area, node, it);
		if (addr >= vma->base && addr < vma->top)
			return vma;
	}
	return nullptr;
}

int vma_map(struct process *p, uintptr_t base, size_t len, int prot, int flags,
	struct vmo *vmo, size_t vmo_off) {
	if (!p || !vmo)
		return -1;
	len = ROUND_UP(len, PAGE_SIZE);

	struct vm_area *vma = malloc(sizeof(*vma));
	memset(vma, 0, sizeof(*vma));
	vma->base = base;
	vma->top = base + len;
	vma->prot = prot;
	vma->flags = flags;
	vma->vmo_off = vmo_off;
	vma->vmo = vmo;
	vmo_ref(vmo);

	list_append(&p->vmas, &vma->node);

	int pte_flags = 0;
	if (base < VMM_KERNEL_BASE)
		pte_flags |= PAGE_USERMODE;
	vmm_create_unbacked_range(base, len, pte_flags);

	return 0;
}

void vma_unmap_all(struct process *p) {
	if (!p)
		return;
	list_for_each_safe (&p->vmas) {
		struct vm_area *vma = container_of(struct vm_area, node, it);
		vmm_unmap_range(vma->base, vma->top - vma->base);
		list_remove(&vma->node);
		vmo_unref(vma->vmo);
		free(vma);
	}
}

void vma_drop_list(struct process *p) {
	if (!p)
		return;
	list_for_each_safe (&p->vmas) {
		struct vm_area *vma = container_of(struct vm_area, node, it);
		list_remove(&vma->node);
		vmo_unref(vma->vmo);
		free(vma);
	}
}

void vma_clone_list(struct process *dst, struct process *src) {
	if (!dst || !src)
		return;
	list_for_each (&src->vmas) {
		struct vm_area *vma = container_of(struct vm_area, node, it);
		struct vm_area *copy = malloc(sizeof(*copy));
		memcpy(copy, vma, sizeof(*copy));
		list_init(&copy->node);
		vmo_ref(copy->vmo);
		list_append(&dst->vmas, &copy->node);
	}
}
