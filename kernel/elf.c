
#include <basic.h>
#include <stddef.h>
#include <stdint.h>
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

int check_elf(Elf64_Ehdr *elf) {
    uint8_t working_ident[8] = {
        0x7F, 'E', 'L', 'F',
        ELF64, ELFLE, ELFABI, ELFEXEC
    };

    if (memcmp(elf, working_ident, 8) == 0) {
        return 1;
    } else {
        return 0;
    }
}

int load_elf(Elf64_Ehdr *elf) {
    Elf64_Phdr *phdr = ((void *)elf) + elf->e_phoff;

    for (int i=0; i<elf->e_phnum; i++) {
        if (phdr[i].p_type != PT_LOAD)
            continue;

        printf("    loading file:%#010lx+%#06lx -> %#010lx %s%s%s\n",
                phdr[i].p_offset, phdr[i].p_memsz, phdr[i].p_vaddr,

                phdr[i].p_flags & PF_R ? "r" : "-",
                phdr[i].p_flags & PF_W ? "w" : "-",
                phdr[i].p_flags & PF_X ? "x" : "-");

        uintptr_t page = phdr[i].p_vaddr & PAGE_MASK_4K;
        for (size_t off = 0; off <= phdr[i].p_memsz; off += 0x1000) {
            vmm_create_unbacked(page + off, PAGE_USERMODE | PAGE_WRITEABLE);
        }

        memcpy(phdr[i].p_vaddr, ((void *)elf) + phdr[i].p_offset, phdr[i].p_memsz);
    }
}

