
#include <basic.h>
#include <ng/debug.h>
#include <ng/fs.h>
#include <ng/multiboot.h>
#include <ng/mutex.h>
#include <ng/panic.h>
#include <ng/pmm.h>
#include <nc/stdio.h>

uintptr_t pmm_first_free_page;
uintptr_t pmm_last_page;
uintptr_t pmm_free_stack_size = 0;

/*
 *
 * TODO
 *
 * free stack
 *
 */

static kmutex pmm_lock = KMUTEX_INIT;

int physical_pages_allocated_total = 0;
int physical_pages_freed_total = 0;

struct pmm_region {
        uintptr_t addr;
        uintptr_t top;
};

struct pmm_region available_regions[32] = {{0}};
int in_region = 0;
uintptr_t top_free_page;
bool regions_oom = false;

void pmm_mmap_cb(uintptr_t addr, uintptr_t len, int type) {
        static int region = 0;
        if (region > 31) {
                printf("got too many regions for pmm to save them all\n");
                return;
        }
        if (addr >= 0x100000 && type == 1) {
                available_regions[region].addr = addr;
                available_regions[region].top = addr + len;
                region += 1;
        }
}

void pmm_allocator_init(uintptr_t first_avail) {
        top_free_page = first_avail;

        mb_mmap_enumerate(pmm_mmap_cb);
}

uintptr_t raw_pmm_allocate_page() {
        uintptr_t ret = top_free_page;
        top_free_page += 0x1000;

        if (regions_oom) {
                printf("You OOM'd - here's the stats:\n");
                printf("allocated: %i\n", physical_pages_allocated_total);
                printf("freed:     %i\n", physical_pages_freed_total);
                panic("implement a pmm free list - OOM\n");
        }

        if (top_free_page == available_regions[in_region].top) {
                if (available_regions[in_region + 1].addr) {
                        in_region += 1;
                } else {
                        regions_oom = true;
                }
        }

        physical_pages_allocated_total += 1;

        return ret;
}

uintptr_t pmm_allocate_page() {
        await_mutex(&pmm_lock);
        uintptr_t page = raw_pmm_allocate_page();
        release_mutex(&pmm_lock);
        return page;
}

void pmm_free_page(uintptr_t addr) {
        physical_pages_freed_total += 1;
}

uintptr_t pmm_allocate_contiguous(int count) {
        // TODO: if I ever use free lists this will need to change

        await_mutex(&pmm_lock);

        uintptr_t page1 = raw_pmm_allocate_page();

        for (int i = 0; i < count; i++) {
                raw_pmm_allocate_page();
        }

        release_mutex(&pmm_lock);
        return page1;
}

void pmm_procfile(struct open_file *ofd) {
        ofd->buffer = malloc(128);
        int count = sprintf(ofd->buffer, "%i %i\n",
                        physical_pages_allocated_total,
                        physical_pages_freed_total);
        ofd->length = count;
}

/*
 * Physical memory map
 *
 * an array representing each physical page in the system so I can track
 * refcounts in from the virtual memory system and some metadata
 *
 */

typedef int16_t pmm_e;

// 0 means not in use
// positive are refcounts (# of VM maps with this page)
// negative are special conditions, as below

const pmm_e KERNEL = -1;
const pmm_e PERMANENT = -2;     // initfs for now
const pmm_e LEAK = -100;        // refcount overflow, can't ever free.

pmm_e *pmm_map;
ssize_t pmm_map_length;

pmm_e *ptr_to_map_entry(void *ptr) {
        if (!pmm_map) return NULL;
        uintptr_t p = (uintptr_t)ptr;
        size_t off = p / PAGE_SIZE;
        return pmm_map + off;
}

void pmm_incref(uintptr_t page);
void pmm_pincref(void *page);
void pmm_decref(uintptr_t page);
void pmm_pdecref(void *page);
void pmm_incref_range(uintptr_t page, ssize_t len);
void pmm_pincref_range(void *page, ssize_t len);
void pmm_decref_range(uintptr_t page, ssize_t len);
void pmm_pdecref_range(void *page, ssize_t len);

void pmm_init_map(uintptr_t highest_physical_page) {
        pmm_map_length = highest_physical_page / PAGE_SIZE * sizeof(pmm_e);

        pmm_map = vmm_reserve(pmm_map_length);

        for (int i=0; i<pmm_map_length; i++) {
                pmm_map[i] = 0;
        }

        extern char _kernel_begin;
        extern char _kernel_end;

        size_t kernel_begin_offset = ptr_to_map_entry(&_kernel_begin);
        size_t kernel_end_offset = ptr_to_map_entry(&_kernel_end);
        
        for (int i=kernel_begin_offset; i<kernel_end_offset; i++) {
                pmm_map[i] = KERNEL;
        }
}

struct region {
        void *ptr;
        size_t len;
};

