
#include <ng/basic.h>
#include <ng/elf.h>
#include <ng/panic.h>
#include <ng/print.h>
#include <ng/string.h>
#include <ng/vmm.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void elf_debugprint(Elf *elf) {
        int bits = elf_verify(elf);
        if (bits == 0) {
                printf("invalid elf\n");
                return;
        }
        Elf *hdr = elf;
        printf("elf64:\n");
        printf("  entrypoint: %#lx\n", hdr->e_entry);
        printf("  phdr      : %#lx\n", hdr->e_phoff);
        printf("  phnum     : %#lx\n", hdr->e_phnum);

        char *phdr_l = ((char *)hdr) + hdr->e_phoff;
        Elf_Phdr *phdr = (Elf_Phdr *)phdr_l;

        for (int i = 0; i < hdr->e_phnum; i++) {
                if (phdr[i].p_type != PT_LOAD)
                        continue;

                printf(
                    "    load file:%#010zx+%#06zx -> %#010zx %s%s%s\n",
                    phdr[i].p_offset, phdr[i].p_memsz, phdr[i].p_vaddr,

                    phdr[i].p_flags & PF_R ? "r" : "-",
                    phdr[i].p_flags & PF_W ? "w" : "-",
                    phdr[i].p_flags & PF_X ? "x" : "-");
        }
}

const char elf64_header_example[8] = {
    0x7F, 'E', 'L', 'F', ELF64, ELFLE, ELFVERSION, ELFABI,
};

const char elf32_header_example[8] = {
    0x7F, 'E', 'L', 'F', ELF32, ELFLE, ELFVERSION, ELFABI,
};

int elf_verify(Elf *elf) {
        if (memcmp(elf, elf64_header_example, 8) == 0) {
                return 64;
        } else if (memcmp(elf, elf32_header_example, 8) == 0) {
                return 32;
        } else {
                return 0;
        }
}

void init_section(void *dst_vaddr, size_t len) {
        uintptr_t bot = round_down((uintptr_t)dst_vaddr, 0x1000);
        uintptr_t top = round_up((uintptr_t)dst_vaddr + len, 0x1000);

        for (uintptr_t p = bot; p <= top; p += 0x1000)
                vmm_create_unbacked(p, PAGE_USERMODE | PAGE_WRITEABLE);
}

void load_section(void *dst_vaddr, void *src_vaddr, size_t flen, size_t mlen) {
        memcpy(dst_vaddr, src_vaddr, flen);

        // BSS is specified by having p_memsz > p_filesz
        // you are expected to zero the extra space
        if (mlen > flen) {
                memset((char *)dst_vaddr + flen, 0, mlen - flen);
        }
}

void *elf_at(Elf *elf, size_t offset) { return ((char *)elf) + offset; }

int elf_load(Elf *elf) {
        int bits = elf_verify(elf);
        if (bits == 0) {
                printf("invalid elf\n");
                return -1;
        }
        char *phdr_l = ((char *)elf) + elf->e_phoff;
        Elf_Phdr *phdr = (Elf_Phdr *)phdr_l;

        for (int i = 0; i < elf->e_phnum; i++) {
                Elf_Phdr *sec = &phdr[i];
                if (sec->p_type != PT_LOAD)
                        continue;
                void *section = elf_at(elf, sec->p_offset);

                init_section((void *)sec->p_vaddr, sec->p_memsz);
                load_section((void *)sec->p_vaddr, section,
                             sec->p_filesz, sec->p_memsz);
        }
        return 0;
}

void elf_print_syms(Elf *elf) {
}

void *elf_get_sym(const char *sym_name, Elf *elf) {
        return NULL;
}

