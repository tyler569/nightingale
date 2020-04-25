
#include <basic.h>
#include <ng/bitmap.h>
#include <ng/multiboot2.h>
#include <ng/mutex.h>
#include <ng/pmm.h>
#include <nc/assert.h>
#include <nc/list.h>
#include <nc/stdio.h>
#include <nc/stdlib.h>

mutex_t pmm_lock = KMUTEX_INIT;

struct pm_region {
        phys_addr_t base;
        phys_addr_t top;
        int pages;
        int pages_free;
        enum pm_state state;
        list_node node;
        struct bitmap *bitmap;
};

list reserved_regions = {0};
list onepage_regions = {0};
list free_regions = {0};


struct pm_region *pm_new_mb_region(phys_addr_t base, size_t len, int type) {
        struct pm_region *region = heap_malloc(&early_heap, sizeof(*region));
        size_t annoying_offset = base % PAGE_SIZE;
        size_t real_len;
        if (annoying_offset > len) {
                real_len = 0;
        } else {
                real_len = len - annoying_offset;
        }

        region->base = round_up(base, PAGE_SIZE);
        region->top = base + round_down(real_len, PAGE_SIZE);
        region->pages = real_len / PAGE_SIZE;

        if (region->pages <= 0) {
                // meh, I'm not tracking that. Anything not accounted for
                // will never be used anyway.
                heap_free(&early_heap, region);
                return NULL;
        }
        
        if (type != MULTIBOOT_MEMORY_AVAILABLE) {
                region->state = PM_HWRESERVED;
                _list_append(&reserved_regions, &region->node);
        } else {
                region->state = PM_FREE;
                _list_append(&free_regions, &region->node);
        }
        return region;
}

void pm_mb_init(multiboot_tag_mmap *mmap) {
        mutex_await(&pmm_lock);

        list_init(&reserved_regions);
        list_init(&onepage_regions);
        list_init(&free_regions);

        multiboot_mmap_entry *m = mmap->entries;
        size_t map_len = mmap->size / sizeof(multiboot_mmap_entry);

        for (size_t i=0; i<map_len; i++) {
                pm_new_mb_region(m[i].addr, m[i].len, m[i].type);
        }
        mutex_unlock(&pmm_lock);
}


// A REGION CAN BE:
//   - available for single page allocations
//   - available for contiguous allocations
//   - reserved
//
// there are lists of each kind of region. -- should they be sorted?
// if there are no single page allocations available, a new contiguous range
//   is acquired.

size_t pm_region_len(struct pm_region *pr) {
        return pr->pages * PAGE_SIZE;
}

/*
 * Split r *after* `pages` pages and return the new region
 *
 * Don't call this if the region has been carved up for onepage allocations,
 * the bitmap won't transfer.
 */
struct pm_region *pm_region_split(struct pm_region *r, int pages) {
        struct pm_region *new = heap_malloc(&early_heap, sizeof(*new));

        assert(r->pages > pages);
        assert(r->state == PM_FREE);

        new->base = r->base + pages * PAGE_SIZE;
        new->top = r->top;
        new->pages = r->pages - pages;
        new->state = PM_FREE;

        r->top = new->base;
        r->pages = pages;
        return new;
}

bool pm_region_overlaps(struct pm_region *region, phys_addr_t base, phys_addr_t top) {
        return (region->base <= top && base < region->top);
}

void pm_reserve_overlap(struct pm_region *pm, phys_addr_t base, phys_addr_t top,
                enum pm_state reason) {
        list_remove(&pm->node);

        struct pm_region *left = NULL;
        struct pm_region *right = NULL;

        int pages_to_left = (base - pm->base) / PAGE_SIZE;
        if (pages_to_left < 0)  pages_to_left = 0;
        int pages_to_right = (pm->top - top) / PAGE_SIZE;
        if (pages_to_right < 0)  pages_to_right = 0;

        if (pages_to_left) {
                left = pm;
                pm = pm_region_split(pm, pages_to_left);
        }
        if (pages_to_right) {
                // TODO XXX: this should be the actual page length _of the overlap_
                size_t split = round_up(top - base, PAGE_SIZE) / PAGE_SIZE;
                right = pm_region_split(pm, split);
        }

        if (left) {
                _list_append(&free_regions, &left->node);
        }
        if (right) {
                _list_append(&free_regions, &right->node);
        }

        pm->state = reason;
        _list_append(&reserved_regions, &pm->node);
}

void pm_reserve(phys_addr_t base, phys_addr_t top, enum pm_state reason) {
        struct pm_region *pr, *tmp;
        bool found_containing = false;
        list_foreach_safe(&free_regions, pr, tmp, node) {
                if (pm_region_overlaps(pr, base, top)) {
                        pm_reserve_overlap(pr, base, top, reason);
                }
        }
}


phys_addr_t pm_onepage_alloc(struct pm_region *pr) {
        assert(pr->state == PM_ONEPAGE);
        assert(pr->pages_free > 0);

        long avail = bitmap_take(pr->bitmap);
        assert(avail > 0);

        phys_addr_t new_page = pr->base + avail * PAGE_SIZE;
        pr->pages_free--;

        return new_page;
}

/*
pm_alloc_page:
  - if list_head(onepage_regions):
      allocate from that one
    else
      take a region from free_regions.
      if that region is > 1024 pages, split it
      put the child region on the onepage_regions list
      take a page from it
*/
#define PM_ONEPAGE_THRESHOLD 1024

phys_addr_t pm_alloc_page() {
        mutex_await(&pmm_lock);
        struct pm_region *r;
        r = list_head_entry(struct pm_region, &onepage_regions, node);
        if (r) {
                phys_addr_t page = pm_onepage_alloc(r);
                mutex_unlock(&pmm_lock);
                return page;
        }

        // try for a new full region
        r = list_head_entry(struct pm_region, &free_regions, node);
        if (r) {
                list_remove(&r->node);
                if (r->pages > PM_ONEPAGE_THRESHOLD) {
                        struct pm_region *rest = pm_region_split(r, PM_ONEPAGE_THRESHOLD);
                        _list_append(&free_regions, &rest->node);
                }
                r->state = PM_ONEPAGE;
                r->pages_free = r->pages;
                r->bitmap = bitmap_new_early(r->pages);

                phys_addr_t page = pm_onepage_alloc(r);
                mutex_unlock(&pmm_lock);
                return page;
        }

        // guess we're out of memory :shrug:
        panic("pmm: OOM\n");
}

phys_addr_t pm_alloc_contiguous(int pages) {
        struct pm_region *r;
        bool found = false;
        list_foreach(&free_regions, r, node) {
                if (r->pages >= pages) {
                        found = true;
                        break;
                }
        }

        if (!found) {
                // impossible to satisfy this request
                return PM_NULL;
        }

        list_remove(&r->node);
        if (r->pages > pages) {
                struct pm_region *new = pm_region_split(r, pages);
                _list_append(&free_regions, &new->node);
        }
        r->state = PM_CONTIGUOUS;
        _list_append(&reserved_regions, &r->node);
        return r->base;
}


// Debug

static void print_region(struct pm_region *r) {
        printf("  pm_region {\n");
        printf("    base: %# 18zx\n", r->base);
        printf("    top : %# 18zx\n", r->top);
        printf("    state: ");
        switch (r->state) {
        case PM_HWRESERVED:
                printf("hwreserved\n"); break;
        case PM_KERNEL:
                printf("kernel\n"); break;
        case PM_MULTIBOOT:
                printf("multiboot\n"); break;
        case PM_INITFS:
                printf("initfs\n"); break;
        case PM_FREE:
                printf("free\n"); break;
        default:
                printf("other!\n"); break;
        }
        printf("  }\n");
}

void pm_dump() {
        printf("Reserved regions:\n");

        struct pm_region *r;
        list_foreach(&reserved_regions, r, node) {
                print_region(r);
        }

        printf("Free regions:\n");
        
        list_foreach(&free_regions, r, node) {
                print_region(r);
        }
}

uintptr_t pmm_allocate_page() {
        return pm_alloc_page();
}

void pmm_free_page(uintptr_t page) {
}

uintptr_t pmm_allocate_contiguous(int count) {
        return pm_alloc_contiguous(count);
}

