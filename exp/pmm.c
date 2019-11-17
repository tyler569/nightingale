
#include <basic.h>
#include <ng/debug.h>
#include <ng/multiboot.h>
#include <ng/mutex.h>
#include <ng/panic.h>
#include <ng/pmm.h>
#include <nc/stdio.h>

enum pmm_page_states {
        PMM_PAGE_UNKNOWN   = -1,
        PMM_PAGE_ILLEGAL   = -2,
        PMM_PAGE_KERNEL    = -3,
        PMM_PAGE_INITFS    = -4,
        PMM_PAGE_LEAK      = -5,
};

static kmutex pmm_lock = KMUTEX_INIT;

ssize_t page_count = 0;
int32_t page_refcounts = NULL;

int32_t pmm_first_free = -1;

bool use_early_pmm = true;
uintptr_t early_pmm_base = 0;
uintptr_t early_pmm_current = 0;

void init_pmm_allocator(uintptr_t low_page, uintptr_t high_page) {
        early_pmm_base = early_pmm_current = low_page;

        page_count = high_page >> 12;
        page_refcounts = vmm_reserve(page_count * sizeof(int32_t));

        mb_mmap_enumerate(pmm_set_callback);

        extern char _mapped_kernel_start;
        extern char _kernel_end;

        uintptr_t kernel_start_page = (uintptr_t)&_mapped_kernel_start;
        uintptr_t kernel_end_page = (uintptr_t)&_kernel_end;

        ssize_t kernel_start_ix = kernel_start_page >> 12;
        ssize_t kernel_end_ix = kernel_end_page >> 12;

        ssize_t low_page_ix = low_page >> 12;

        for (ssize_t i=kernel_start_ix; i<kernel_end_ix; i++) {
                page_refcounts[i] = PMM_PAGE_KERNEL;
        }

        for (ssize_t i=kernel_end_ix+1; i<low_page_ix; i++) {
                page_refcounts[i] = PMM_PAGE_INITFS;
        }
        
        ssize_t early_end_ix = early_pmm_current >> 12;

        for (ssize_t i=low_page_ix+1; i<early_end_ix; i++) {
                page_refcounts[i] = 1;
        }

        use_early_pmm = false;
}

void pmm_set_callback(uintptr_t addr, uintptr_t len, int type) {
        ssize_t page_index = addr >> 12;
        ssize_t page_count = len >> 12;
        ssize_t page_index_end = page_index + page_count;

        if (type != 1) {
                for (ssize_t i=page_index; i<page_index_end; i++) {
                        page_refcounts[i] = PMM_PAGE_ILLEGAL;
                }
        }
}

uintptr_t pmm_allocate() {
        if (use_early_pmm) {
                uintptr_t page = early_pmm_current;
                early_pmm_current += 0x1000;
                return page;
        }

}

void pmm_free(uintptr_t page_frame) {
        if (use_early_pmm) {
                // leak :(
                return;
        }
        ssize_t frame_index = page_frame >> 12;

        if (pmm_first_free > 0) {
                page_refcounts[frame_index] = pmm_first_free;
        }
        pmm_first_free = frame_index;
}

