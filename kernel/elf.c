#include <basic.h>
#include <elf.h>
#include <ng/thread.h>
#include <ng/vmm.h>
#include <string.h>

static const char elf64_header_example[8] = {
    0x7F, 'E', 'L', 'F', ELF64, ELFLE, ELFVERSION, ELFABI,
};

#define VERIFY_DEPTH 4

int elf_verify(Elf_Ehdr *elf) {
    if (memcmp(elf, elf64_header_example, VERIFY_DEPTH) == 0) {
        return 64;
    } else {
        printf("\ntried to load: [ ");
        for (int i=0; i<16; i++) {
            printf("%02hhx ", ((char *)elf)[i]);
        }
        printf("]\n");
        return 0;
    }
}

static void init_section(void *destination_vaddr, size_t len) {
    uintptr_t bot = round_down((uintptr_t)destination_vaddr, 0x1000);
    uintptr_t top = round_up((uintptr_t)destination_vaddr + len, 0x1000);

    user_map(bot, top);
}

static void load_section(void *destination_vaddr, void *source_vaddr,
                         size_t flen, size_t mlen) {
    memcpy(destination_vaddr, source_vaddr, flen);

    // BSS is specified by having p_memsz > p_filesz
    // you are expected to zero the extra space
    if (mlen > flen) {
        memset((char *)destination_vaddr + flen, 0, mlen - flen);
    }
}

int elf_load(void *buffer) {
    int bits = elf_verify(buffer);
    if (bits == 0) {
        printf("invalid elf\n");
        return -1;
    }
    Elf_Ehdr *elf = buffer;
    char *phdr_l = ((char *)elf) + elf->e_phoff;
    Elf_Phdr *phdr = (Elf_Phdr *)phdr_l;

    for (int i = 0; i < elf->e_phnum; i++) {
        Elf_Phdr *sec = &phdr[i];
        if (sec->p_type != PT_LOAD) continue;

        void *section = (char *)buffer + sec->p_offset;

        init_section((void *)sec->p_vaddr, sec->p_memsz);
        load_section((void *)sec->p_vaddr, section, sec->p_filesz,
                     sec->p_memsz);
    }
    return 0;
}

Elf_Sym *elf_symbol_by_address(elf_md *e, uintptr_t address) {
    Elf_Shdr *symtab_header = e->symbol_table_section;
    size_t nsymbols = symtab_header->sh_size / symtab_header->sh_entsize;
    Elf_Sym *symtab = e->symbol_table;

    uintptr_t addr_match = 0;
    Elf_Sym *best_match = NULL;

    for (size_t i = 0; i < nsymbols; i++) {
        Elf_Sym *sym = symtab + i;

        if (sym->st_name == 0) continue;
        if (sym->st_value > address) continue;

        if (sym->st_value > addr_match) {
            best_match = sym;
            addr_match = sym->st_value;
        }
    }

    return best_match;
}
