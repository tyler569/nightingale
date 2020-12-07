#include <basic.h>
#include <elf.h>
#include <ng/thread.h>
#include <ng/vmm.h>
#include <string.h>

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

int elf_load(elf_md *e) {
    const Elf_Ehdr *elf = e->imm_header;
    const Elf_Phdr *phdr = e->program_headers;

    for (int i = 0; i < elf->e_phnum; i++) {
        const Elf_Phdr *sec = &phdr[i];
        if (sec->p_type != PT_LOAD) continue;

        void *section = (char *)e->buffer + sec->p_offset;

        init_section((void *)sec->p_vaddr, sec->p_memsz);
        load_section((void *)sec->p_vaddr, section, sec->p_filesz,
                     sec->p_memsz);
    }
    return 0;
}

const Elf_Sym *elf_symbol_by_address(elf_md *e, uintptr_t address) {
    size_t nsymbols = e->symbol_count;
    const Elf_Sym *symtab = e->symbol_table;

    uintptr_t addr_match = 0;
    const Elf_Sym *best_match = NULL;

    for (size_t i = 0; i < nsymbols; i++) {
        const Elf_Sym *sym = symtab + i;

        if (sym->st_name == 0) continue;
        if (sym->st_value > address) continue;

        if (sym->st_value > addr_match) {
            best_match = sym;
            addr_match = sym->st_value;
        }
    }

    return best_match;
}
