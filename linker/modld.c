#include <basic.h>
#include <assert.h>
#include <elf.h>
#include <ng/fs.h>
#include <ng/multiboot2.h>
#include <ng/vmm.h>
#include <stdio.h>
#include <string.h>

#define PTR_ADD(p, off) (void *)(((char *)p) + off)

elf_md elf_ngk_md;

void *elf_sym_addr(elf_md *e, Elf_Sym *sym);
void elf_relo_resolve(elf_md *module, elf_md *main);

void elf_relocate(elf_md *e, Elf_Shdr *modify_section, Elf_Rela *rela) {
    size_t sym_index = ELF64_R_SYM(rela->r_info);
    Elf_Sym *sym = &e->symbol_table[sym_index];
    int rel_type = ELF64_R_TYPE(rela->r_info);
    const char *name = elf_symbol_name(e, sym);

    size_t section_offset = rela->r_offset;
    void *relocation = (char *)modify_section->sh_addr + section_offset;

    uint64_t A = rela->r_addend;
    uint64_t P = (uint64_t)relocation;
    uint64_t S = (uint64_t)elf_sym_addr(e, sym);

    // printf("A: %#lx\n", A);
    // printf("P: %#lx\n", P);
    // printf("S: %#lx\n", S);
    // printf("relocation @ %p ", relocation);
    // switch (rel_type) {
    // case R_X86_64_NONE: printf("none\n"); break;
    // case R_X86_64_64: printf("64: %#lx\n", S + A); break;
    // case R_X86_64_32: printf("32: %#x\n", (uint32_t)(S + A)); break;
    // case R_X86_64_32S: printf("32S: %#x\n", (int32_t)(S + A)); break;
    // case R_X86_64_PC32: // FALLTHROUGH
    // case R_X86_64_PLT32: printf("PC32: %#x\n", (uint32_t)(S + A - P)); break;
    // }

    switch (rel_type) {
    case R_X86_64_NONE: break;
    case R_X86_64_64: *(uint64_t *)relocation = S + A; break;
    case R_X86_64_32: *(uint32_t *)relocation = S + A; break;
    case R_X86_64_32S: *(int32_t *)relocation = S + A; break;
    case R_X86_64_PC32: // FALLTHROUGH
    case R_X86_64_PLT32: *(uint32_t *)relocation = S + A - P; break;
    default: printf("invalid relocation type: %i\n", rel_type);
    }
}

elf_md *elf_relo_load(elf_md *relo) {
    // get needed virtual allocation size (file size + sum of all common
    // symbol sizes)
    size_t file_size = relo->file_size;
    size_t relo_needed_virtual_size = relo->file_size;
    size_t relo_common_size = 0;
    assert(relo_needed_virtual_size > 0);

    assert(relo->image->e_type == ET_REL);
    assert(relo->symbol_table);
    assert(relo->string_table);

    // total bss size needed
    for (int i = 0; i < relo->symbol_count; i++) {
        Elf_Sym *sym = relo->symbol_table + i;
        if (sym->st_shndx == SHN_COMMON) {
            // This math must match the bss symbol placement below
            relo_common_size = round_up(relo_common_size, sym->st_value);
            relo_common_size += sym->st_size;
        }
    }
    relo_needed_virtual_size += relo_common_size;

    void *relo_load = malloc(relo_needed_virtual_size);

    memcpy(relo_load, relo->mem, relo->file_size);
    memset(relo->bss_base, 0, relo_common_size);

    free(relo);
    relo = elf_parse(relo_load);

    relo->file_size = file_size;
    relo->load_mem = relo_load;
    relo->bss_base = PTR_ADD(relo_load, relo->file_size);

    // Set shdr->sh_addr to their loaded addresses.
    for (int i = 0; i < relo->image->e_shnum; i++) {
        Elf_Shdr *shdr = &relo->section_headers[i];
        shdr->sh_addr = shdr->sh_offset + (uintptr_t)relo->load_mem;
    }

    Elf_Shdr *bss = elf_find_section(relo, ".bss");
    bss->sh_addr = (uintptr_t)relo->bss_base;

    // Place bss symbols' st_values in bss region.
    for (int i = 0; i < relo->symbol_count; i++) {
        size_t bss_offset = 0;
        Elf_Sym *sym = relo->symbol_table + i;
        if (sym->st_shndx == SHN_COMMON) {
            // This math must match the bss size calculation above
            // (relo_common_size)
            size_t value = sym->st_value;
            sym->st_value = round_up(bss_offset, value);
            bss_offset += sym->st_size;
        }
    }

    elf_relo_resolve(relo, &elf_ngk_md);

    for (int i = 0; i < relo->image->e_shnum; i++) {
        Elf_Shdr *sec = relo->section_headers + i;
        if (sec->sh_type != SHT_RELA) { continue; }

        Elf_Shdr *modify_section = relo->section_headers + sec->sh_info;
        size_t section_size = sec->sh_size;
        size_t rela_count = section_size / sizeof(Elf_Rela);
        for (size_t i = 0; i < rela_count; i++) {
            Elf_Rela *rela = (Elf_Rela *)sec->sh_addr + i;
            elf_relocate(relo, modify_section, rela);
        }
    }

    return relo;
}

// For all as-yet undefined symbols in `module`, resolve them to a
// equivalently-named symbol in `main`
void elf_relo_resolve(elf_md *module, elf_md *main) {
    for (size_t i = 0; i < module->symbol_count; i++) {
        Elf_Sym *sym = &module->symbol_table[i];
        int type = ELF_ST_TYPE(sym->st_info);
        if (type == STT_FILE) continue;
        if (sym->st_shndx == SHN_UNDEF) {
            const char *name = elf_symbol_name(module, sym);
            if (!name || !name[0]) continue;
            Elf_Sym *psym = elf_find_symbol(main, name);
            sym->st_value = psym->st_value;
            sym->st_info = psym->st_info;
            sym->st_shndx = SHN_ABS;
            printf("resolved '%s' -> %p\n", name, sym->st_value);
        }
    }
}

void *elf_sym_addr(elf_md *e, Elf_Sym *sym) {
    if (sym->st_shndx == SHN_COMMON) {
        return (char *)e->bss_base + sym->st_value;
    }
    Elf_Shdr *section = &e->section_headers[sym->st_shndx];
    return (char *)section->sh_addr + sym->st_value;
}

void load_kernel_elf(multiboot_tag_elf_sections *mb_sym) {
    elf_md *ngk = &elf_ngk_md;

    ngk->section_header_count = mb_sym->num;
    ngk->section_headers = (Elf_Shdr *)&mb_sym->sections;
    ngk->shdr_string_table_section = ngk->section_headers + mb_sym->shndx;
    if (ngk->shdr_string_table_section) {
        ngk->shdr_string_table =
            (const char *)(ngk->shdr_string_table_section->sh_addr +
                           VMM_KERNEL_BASE);
    }

    ngk->symbol_table_section = elf_find_section(ngk, ".symtab");
    ngk->string_table_section = elf_find_section(ngk, ".strtab");

    if (ngk->string_table_section) {
        ngk->string_table = (const char *)(ngk->string_table_section->sh_addr +
                                           VMM_KERNEL_BASE);
    }

    if (ngk->symbol_table_section) {
        ngk->symbol_table =
            (Elf_Sym *)(ngk->symbol_table_section->sh_addr + VMM_KERNEL_BASE);
    }
}

elf_md *elf_mod_load(struct file *elf_file) {
    assert(elf_file->filetype == FT_BUFFER);
    struct membuf_file *elf_membuf = (struct membuf_file *)elf_file;
    elf_md *mod = elf_parse(elf_membuf->memory);
    mod->file_size = elf_file->len;

    mod = elf_relo_load(mod);

    return mod;
}
