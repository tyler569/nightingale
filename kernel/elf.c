
#include <basic.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <panic.h>
#include <print.h>
#include <vmm.h>
#include "elf.h"

void elf_debugprint(void* elf) {
    int bits = elf_verify(elf);
    if (bits == 0) {
        printf("invalid elf\n");
        return;
    }
    if (bits == 64) {
        Elf64_Ehdr* hdr = elf;
        printf("elf64:\n");
        printf("  entrypoint: %#lx\n", hdr->e_entry);
        printf("  phdr      : %#lx\n", hdr->e_phoff);
        printf("  phnum     : %#lx\n", hdr->e_phnum);

        char* phdr_l = ((char*)hdr) + hdr->e_phoff;
        Elf64_Phdr* phdr = (Elf64_Phdr*)phdr_l;

        for (int i=0; i<hdr->e_phnum; i++) {
            if (phdr[i].p_type != PT_LOAD)
                continue;

            printf("    load file:%#010lx+%#06lx -> %#010lx %s%s%s\n",
                    phdr[i].p_offset, phdr[i].p_memsz, phdr[i].p_vaddr,

                    phdr[i].p_flags & PF_R ? "r" : "-",
                    phdr[i].p_flags & PF_W ? "w" : "-",
                    phdr[i].p_flags & PF_X ? "x" : "-");
        }
    } else if (bits == 32) {
        Elf32_Ehdr* hdr = elf;
        printf("elf32:\n");
        printf("  entrypoint: %#lx\n", hdr->e_entry);
        printf("  phdr      : %#lx\n", hdr->e_phoff);
        printf("  phnum     : %#lx\n", hdr->e_phnum);

        char* phdr_l = ((char*)hdr) + hdr->e_phoff;
        Elf32_Phdr* phdr = (Elf32_Phdr*)phdr_l;

        for (int i=0; i<hdr->e_phnum; i++) {
            if (phdr[i].p_type != PT_LOAD)
                continue;

            printf("    load file:%#08lx+%#06lx -> %#08lx %s%s%s\n",
                    phdr[i].p_offset, phdr[i].p_memsz, phdr[i].p_vaddr,

                    phdr[i].p_flags & PF_R ? "r" : "-",
                    phdr[i].p_flags & PF_W ? "w" : "-",
                    phdr[i].p_flags & PF_X ? "x" : "-");
        }
    }
}

const char elf64_header_example[8] = {
    0x7F, 'E', 'L', 'F',
    ELF64, ELFLE, ELFVERSION, ELFABI,
};

const char elf32_header_example[8] = {
    0x7F, 'E', 'L', 'F',
    ELF32, ELFLE, ELFVERSION, ELFABI,
};

int elf_verify(void* elf) {
    if (memcmp(elf, elf64_header_example, 8) == 0) {
        return 64;
    } else if (memcmp(elf, elf32_header_example, 8) == 0) {
        return 32;
    } else {
        return 0;
    }
}

int elf_load(void* elf_) {
    int bits = elf_verify(elf_);
    if (bits == 0) {
        printf("invalid elf\n");
        return -1;
    }
    if (bits == 64) {
        Elf64_Ehdr* elf = elf_;
        char* phdr_l = ((char*)elf) + elf->e_phoff;
        Elf64_Phdr* phdr = (Elf64_Phdr*)phdr_l;

        for (int i=0; i<elf->e_phnum; i++) {
            if (phdr[i].p_type != PT_LOAD)
                continue;

            uintptr_t page = phdr[i].p_vaddr & PAGE_MASK_4K;
            for (size_t off = 0; off <= phdr[i].p_memsz; off += 0x1000) {
                vmm_create_unbacked(page + off, PAGE_USERMODE | PAGE_WRITEABLE);
                // if the pages already exist, they are recycled, since creating an
                // existing page is a noop and COW forks are a thing
            }

            memcpy((char*)phdr[i].p_vaddr, ((char*)elf) + phdr[i].p_offset, phdr[i].p_memsz);
        }
        return 0;
    } else if (bits == 32) {
        Elf32_Ehdr* elf = elf_;
        char* phdr_l = ((char*)elf) + elf->e_phoff;
        Elf32_Phdr* phdr = (Elf32_Phdr*)phdr_l;

        for (int i=0; i<elf->e_phnum; i++) {
            if (phdr[i].p_type != PT_LOAD)
                continue;

            uintptr_t page = phdr[i].p_vaddr & PAGE_MASK_4K;
            for (size_t off = 0; off <= phdr[i].p_memsz; off += 0x1000) {
                vmm_create_unbacked(page + off, PAGE_USERMODE | PAGE_WRITEABLE);
                // if the pages already exist, they are recycled, since creating an
                // existing page is a noop and COW forks are a thing
            }

            memcpy((char*)phdr[i].p_vaddr, ((char*)elf) + phdr[i].p_offset, phdr[i].p_memsz);
        }
        return 0;
    } else {
        return -1;
    }
}

