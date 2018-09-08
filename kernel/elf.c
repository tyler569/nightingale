
#include <basic.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <print.h>
#include <vmm.h>
#include "elf.h"

void print_elf(Elf64_Ehdr *elf) {
    printf("elf:\n");
    printf("  entrypoint: %#lx\n", elf->e_entry);
    printf("  phdr      : %#lx\n", elf->e_phoff);
    printf("  phnum     : %#lx\n", elf->e_phnum);

    Elf64_Phdr *phdr = ((void *)elf) + elf->e_phoff;

    for (int i=0; i<elf->e_phnum; i++) {
        if (phdr[i].p_type != PT_LOAD)
            continue;

        printf("    load file:%#010lx+%#06lx -> %#010lx %s%s%s\n",
                phdr[i].p_offset, phdr[i].p_memsz, phdr[i].p_vaddr,

                phdr[i].p_flags & PF_R ? "r" : "-",
                phdr[i].p_flags & PF_W ? "w" : "-",
                phdr[i].p_flags & PF_X ? "x" : "-");
    }
}

bool check_elf(Elf64_Ehdr *elf) {
    uint8_t working_ident[8] = {
        0x7F, 'E', 'L', 'F',
        ELF64, ELFLE, ELFABI, ELFEXEC
    };

    return memcmp(elf, working_ident, 8) == 0;
}

int load_elf(Elf64_Ehdr *elf) {
    Elf64_Phdr *phdr = ((void *)elf) + elf->e_phoff;

    for (int i=0; i<elf->e_phnum; i++) {
        if (phdr[i].p_type != PT_LOAD)
            continue;

//#define __ng_print_load_elf
#ifdef __ng_print_load_elf
        printf("    loading file:%#010lx+%#06lx -> %#010lx %s%s%s\n",
                phdr[i].p_offset, phdr[i].p_memsz, phdr[i].p_vaddr,

                phdr[i].p_flags & PF_R ? "r" : "-",
                phdr[i].p_flags & PF_W ? "w" : "-",
                phdr[i].p_flags & PF_X ? "x" : "-");
#endif

        uintptr_t page = phdr[i].p_vaddr & PAGE_MASK_4K;
        for (size_t off = 0; off <= phdr[i].p_memsz; off += 0x1000) {
            vmm_create_unbacked(page + off, PAGE_USERMODE | PAGE_WRITEABLE);
            // if the pages already exist, they are recycled, since creating an
            // exisint page is a noop and COW forks are a thing
        }

        memcpy((void *)phdr[i].p_vaddr, ((void *)elf) + phdr[i].p_offset, phdr[i].p_memsz);
    }
    return 0;
}

