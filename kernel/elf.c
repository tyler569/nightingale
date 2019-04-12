
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

            printf("    load file:%#010zx+%#06zx -> %#010zx %s%s%s\n",
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

            printf("    load file:%#010zx+%#06zx -> %#010zx %s%s%s\n",
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


void init_section(void* dst_vaddr, size_t len) {
    uintptr_t bot = round_down((uintptr_t)dst_vaddr, 0x1000);
    uintptr_t top = round_up((uintptr_t)dst_vaddr + len, 0x1000);

    for (uintptr_t p = bot; p<=top; p += 0x1000)
        vmm_create_unbacked(p, PAGE_USERMODE | PAGE_WRITEABLE);
}

void load_section(void* dst_vaddr, void* src_vaddr, size_t flen, size_t mlen) {
    memcpy(dst_vaddr, src_vaddr, flen);

    // BSS is specified by having p_memsz > p_filesz
    // you are expected to zero the extra space
    if (mlen > flen) {
        memset((char*)dst_vaddr + flen, 0, mlen - flen);
    }
}

void* elf_at(void* elf, size_t offset) {
    return ((char*)elf) + offset;
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
            Elf64_Phdr* sec = &phdr[i];
            if (sec->p_type != PT_LOAD)
                continue;
            void* section = elf_at(elf, sec->p_offset);

            init_section((void*)sec->p_vaddr, sec->p_memsz);
            load_section((void*)sec->p_vaddr, section, sec->p_filesz, sec->p_memsz);
        }
        return 0;
    } else if (bits == 32) {
        Elf32_Ehdr* elf = elf_;
        char* phdr_l = ((char*)elf) + elf->e_phoff;
        Elf32_Phdr* phdr = (Elf32_Phdr*)phdr_l;

        for (int i=0; i<elf->e_phnum; i++) {
            Elf32_Phdr* sec = &phdr[i];
            if (sec->p_type != PT_LOAD)
                continue;
            void* section = elf_at(elf, sec->p_offset);

            init_section((void*)sec->p_vaddr, sec->p_memsz);
            load_section((void*)sec->p_vaddr, section, sec->p_filesz, sec->p_memsz);
        }
        return 0;
    } else {
        return -1;
    }
}

