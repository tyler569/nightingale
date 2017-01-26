
#include <stdint.h>

#include <kernel/cpu.h>

uint32_t page_directory[1024] __attribute__((aligned(4096)));
uint32_t page_table_1[1024] __attribute__((aligned(4096)));

void setup_paging() {
    page_directory[0] = 0;

    for (int i=0; i < 1024; i++) {
        page_directory[i] = 0x00000002;
    }
    for (int i=0; i < 1024; i++) {
        page_table_1[i] = (i * 0x1000) | 3;
    }
    page_directory[0] = ((uint32_t)page_table_1 | 3);
    
    __asm__ ( "mov %0, %%eax \n\t"
              "mov %%eax, %%cr3"
              : : "r" (page_directory));
    
    __asm__ ( "mov %cr0, %eax \n\t"
              "or $0x80000000, %eax \n\t"
              "mov %eax, %cr0" );

}
