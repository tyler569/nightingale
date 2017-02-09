
/* 
 */

//void paging_system_init(multiboot_info_t *mbdata);

int paging_mem_available();

uintptr_t vma_to_pma(uintptr_t vma);

/* TBI */

void map_page(uintptr_t phy, uintptr_t virt);
void map_4M_page(uintptr_t phy, uintptr_t virt);

void unmap_page(uintptr_t virt);




