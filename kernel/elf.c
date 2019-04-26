
#include <ng/basic.h>
#include <ng/elf.h>
#include <ng/panic.h>
#include <ng/print.h>
#include <ng/string.h>
#include <ng/vmm.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h> 

void elf_debugprint(Elf *elf) { int bits = elf_verify(elf);
        if (bits == 0) {
                printf("invalid elf\n");
                return;
        }
        Elf *hdr = elf;
        printf("elf64:\n");
        printf("  entrypoint: %#lx\n", hdr->e_entry);
        printf("  phdr      : %#lx\n", hdr->e_phoff);
        printf("  phnum     : %#x\n", hdr->e_phnum);

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
        0x7F, 'E', 'L', 'F', ELF64, ELFLE, ELFVERSION, ELFABI
};

const char elf32_header_example[8] = {
    0x7F, 'E', 'L', 'F', ELF32, ELFLE, ELFVERSION, ELFABI
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

void *elf_at(Elf *elf, size_t offset) { return ((char *)elf) + offset; }

Elf_Shdr *elf_get_symtab(Elf *elf) {
        Elf_Shdr *shdr = elf_at(elf, elf->e_shoff);
        char *str_tab = elf_at(elf, shdr[elf->e_shstrndx].sh_offset);

        Elf_Shdr *symtab = NULL;

        for (int i=0; i<elf->e_shnum; i++) {
                if (!shdr[i].sh_size)
                        continue;
                if (strcmp(&str_tab[shdr[i].sh_name], ".symtab") == 0)
                        symtab = &shdr[i];
        }

        return symtab;
}

Elf_Shdr *elf_get_strtab(Elf *elf) {
        Elf_Shdr *shdr = elf_at(elf, elf->e_shoff);
        char *str_tab = elf_at(elf, shdr[elf->e_shstrndx].sh_offset);

        Elf_Shdr *strtab = NULL;

        for (int i=0; i<elf->e_shnum; i++) {
                if (!shdr[i].sh_size)
                        continue;
                if (strcmp(&str_tab[shdr[i].sh_name], ".strtab") == 0)
                        strtab = &shdr[i];
        }

        return strtab;
}

void elf_print_syms(Elf *elf) {
        Elf_Shdr *shdr = elf_at(elf, elf->e_shoff);
        char *str_tab = elf_at(elf, shdr[elf->e_shstrndx].sh_offset);

        Elf_Shdr *strtab = elf_get_strtab(elf);
        Elf_Shdr *symtab = elf_get_symtab(elf);

        if (!strtab || !symtab)
                return;

        Elf_Sym *sym = (Elf_Sym *) (((char *)elf) + symtab->sh_offset);
        char *str = ((char *)elf) + strtab->sh_offset;

        for (int i=0; i<symtab->sh_size / sizeof(Elf_Sym); i++) {
                if (*(str + sym[i].st_name))
                        printf("%s: %lx\n",
                                str + sym[i].st_name, sym[i].st_value);
        }
}

Elf_Sym *elf_get_sym_p(const char *sym_name, Elf *elf) {
        Elf_Shdr *shdr = elf_at(elf, elf->e_shoff);
        char *str_tab = elf_at(elf, shdr[elf->e_shstrndx].sh_offset);

        Elf_Shdr *strtab = elf_get_strtab(elf);
        Elf_Shdr *symtab = elf_get_symtab(elf);

        Elf_Sym *sym = elf_at(elf, symtab->sh_offset);
        char *str = elf_at(elf, strtab->sh_offset);

        Elf_Sym *result = NULL;

        for (int i=0; i<symtab->sh_size / sizeof(Elf_Sym); i++) {
                if (strcmp(str + sym[i].st_name, sym_name) == 0) {
                        result = &sym[i];
                        break;
                }
        }
        
        return result;
}

Elf_Sym *elf_get_sym_by_ix(Elf *elf, long symindex) {
        Elf_Shdr *symtab = elf_get_symtab(elf);
        Elf_Sym *sym = elf_at(elf, symtab->sh_offset);

        return &sym[symindex];
}

size_t elf_get_sym_off(const char *sym_name, Elf *elf) {
        Elf_Shdr *shdr = elf_at(elf, elf->e_shoff);
        Elf_Sym *sym = elf_get_sym_p(sym_name, elf);

        size_t value = 0;
        if (sym) {
                value += sym->st_value;
                value += shdr[sym->st_shndx].sh_offset;
        }
        return value;
}

void elf64_print_rels_in_section(Elf *elf, Elf_Shdr *shdr) {
        Elf64_Rela *rela = elf_at(elf, shdr->sh_offset);
        char *str_tab = elf_at(elf, shdr[elf->e_shstrndx].sh_offset);

        Elf_Shdr *strtab = elf_get_strtab(elf);
        char *str = elf_at(elf, strtab->sh_offset);

        for (int i=0; i<shdr->sh_size / sizeof(Elf64_Rela); i++) {
                long symindex = ELF64_R_SYM(rela[i].r_info);
                long reltype = ELF64_R_TYPE(rela[i].r_info);

                printf("%lx %lx %lx %lx\n", rela[i].r_offset,
                        symindex, reltype, rela[i].r_addend);

                Elf_Sym *sym = elf_get_sym_by_ix(elf, symindex);
                if (str[sym->st_name])
                        printf(" %li is %s\n", symindex, &str[sym->st_name]);

                printf("value : %p\n", (void *)(sym->st_value));
        }
}

void elf_print_rels(Elf *elf) {
        Elf_Shdr *shdr = elf_at(elf, elf->e_shoff);
        Elf_Shdr *strtab = elf_get_strtab(elf);
        char *str = elf_at(elf, strtab->sh_offset);

        for (int i=0; i<elf->e_shnum; i++) {
                if (shdr[i].sh_type != SHT_RELA) 
                        continue;
                elf64_print_rels_in_section(elf, &shdr[i]);
        }
}


void elf_resolve_symbols_from_elf(Elf *master, Elf *child) {
        Elf_Shdr *shdr = elf_at(child, child->e_shoff);
        Elf_Shdr *symtab = elf_get_symtab(child);
        Elf_Shdr *strtab = elf_get_strtab(child);
        char *str_tab = elf_at(child, shdr[child->e_shstrndx].sh_offset);

        Elf_Sym *sym = elf_at(child, symtab->sh_offset);
        char *str = elf_at(child, strtab->sh_offset);

        for (int i=0; i<symtab->sh_size / sizeof(Elf_Sym); i++) {
                int type = ELF_ST_TYPE(sym[i].st_info);
                if (type == STT_FILE)
                        continue;

                Elf_Shdr *symbol_origin = &shdr[sym[i].st_shndx];
                if (symbol_origin->sh_type == SHT_NULL) {
                        Elf_Sym *master_sym = elf_get_sym_p(&str[sym[i].st_name],
                                                         master);
                        if (!master_sym) {
                                printf("didn't find a symbol that we needed..\n");
                                continue;
                        }

                        sym[i].st_value = master_sym->st_value;
                }
        }
}

const char *rel_type_names[] = {
        [R_X86_64_NONE] = "R_X86_64_NONE",
        [R_X86_64_64] = "R_X86_64_64",
        [R_X86_64_32] = "R_X86_64_32",
        [R_X86_64_32S] = "R_X86_64_32S",
        [R_X86_64_PC32] = "R_X86_64_PC32",
        [R_X86_64_PLT32] = "R_X86_64_PLT32",
};

#if X86_64
int perform_relocations_in_section(Elf *elf, Elf_Shdr *rshdr, uintptr_t new_base) {
        Elf_Shdr *shdr = elf_at(elf, elf->e_shoff);

        printf("%p %p %p\n", elf, shdr, rshdr);

        Elf64_Rela *rela = elf_at(elf, rshdr->sh_offset);
        char *str_tab = elf_at(elf, shdr[elf->e_shstrndx].sh_offset);

        printf(" (section %s)\n", &str_tab[rshdr->sh_name]);

        Elf_Shdr *link_shdr = &shdr[rshdr->sh_info];
        printf("  (links to %s)\n", &str_tab[link_shdr->sh_name]);

        Elf_Shdr *strtab = elf_get_strtab(elf);
        Elf_Shdr *symtab = elf_get_symtab(elf); // TODO - this is bad
        char *str = elf_at(elf, strtab->sh_offset);
        Elf_Sym *rsym = elf_at(elf, symtab->sh_offset);

        for (int i=0; i<rshdr->sh_size / sizeof(Elf64_Rela); i++) {
                // unsigned long loc = link_shdr->sh_addr + rela[i].r_offset;

                int rel_type = ELF64_R_TYPE(rela[i].r_info);
                int symindex = ELF64_R_SYM(rela[i].r_info);
                Elf_Sym *sym = &rsym[symindex];


                unsigned long loc = rela[i].r_offset;
                loc += link_shdr->sh_offset;

                // inside the kernel, the file will already be loaded to it's
                // final location and we can perform the relocations actually
                // in place, and this becomes redundant.
                unsigned long i_loc = loc + new_base;
                unsigned long p_loc = loc + (uintptr_t)elf;

                long value;
                value = shdr[sym->st_shndx].sh_offset;
                value += sym->st_value;
                value += rela[i].r_addend;

                // suuuuuuuuuuuuuuper jank
                if (value < 0x100000) {
                        value += new_base;
                }

                if (rel_type_names[rel_type]) {
                        printf("(%10s) ", &str[sym->st_name]);
                        printf("relocating: %s at %lx with v:%lx\n",
                                        rel_type_names[rel_type], i_loc, value);
                }

                // TODO: check the location is empty and that
                // there was no overflow
                switch(ELF64_R_TYPE(rela[i].r_info)) {
                case R_X86_64_NONE:
                        break;
                case R_X86_64_64:
                        *(uint64_t *)p_loc = value;
                        break;
                case R_X86_64_32:
                        *(uint32_t *)p_loc = value;
                        break;
                case R_X86_64_32S:
                        *(int32_t *)p_loc = value;
                        break;
                case R_X86_64_PC32:
                case R_X86_64_PLT32:
                        value -= (uint64_t)i_loc;
                        printf("  - actually placing %lx\n", value);
                        *(uint32_t *)p_loc = value;
                        break;
                default:
                        printf("invalid relocation type: %li\n",
                                        ELF64_R_TYPE(rela[i].r_info));
                }
        }

        return 0;
}
#elif I686
int perform_relocations_in_section(Elf *elf, Elf_Shdr *shdr, uintptr_t new_base) {
        return -1;
}
#endif

int elf_relocate_object(Elf *elf, uintptr_t new_base) {
        Elf_Shdr *shdr = elf_at(elf, elf->e_shoff);

        for (int i=0; i<elf->e_shnum; i++) {
                if (shdr[i].sh_type != SHT_RELA) 
                        continue;

                // inside relocation shdr
                perform_relocations_in_section(elf, &shdr[i], new_base);
        }
        return 0;
}

