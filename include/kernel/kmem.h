
/* 
 */

#include <stddef.h>



//void paging_system_init(multiboot_info_t *mbdata);

int free_pages_available();

// Note that techncically, -1 can be a valid physical memory address that
// returns from a mapping, but I'm going with this for 2 reasons:
// 1. it's easy to check if the offset (lower 12/22 bits) are all 1's,
//    in which case it could actutually be valid, but so would address - 1
//    and that would not return -1
//      i.e. 0xabcdefff -> 0xffffffff;
//           0xabcdeffe -> 0xfffffffe; necessarily.
//      In an unmapped page that would also return -1, as vould any other
//      offset.
// 2. this is the alternative to making a vma_is_mapped function, effectively
//    requiring me to trawl the same pages twice per lookup (once to get the 
//    Present bit, then return that and do the same lookups again
#define PAGE_NOT_PRESENT -1

#define PAGE_PRESENT    0x01
#define PAGE_MAPS_4M    0x80
#define PAGE_4M_OFFSET  0x3FFFFF
#define PAGE_4M_MASK    (~PAGE_4M_OFFSET)
#define PAGE_4K_OFFSET  0xFFF
#define PAGE_4K_MASK    (~PAGE_4K_OFFSET)

uintptr_t vma_to_pma(uintptr_t vma);

/* TBI */

#define PAGE_ALIGNMENT_ERROR -1
#define PAGE_ALREADY_PRESENT -2
#define PAGE_ALREADY_VACANT  -3

int map_4k_page(uintptr_t vma, uintptr_t pma);
int map_4M_page(uintptr_t vma, uintptr_t pma);

int unmap_page(uintptr_t vma);




