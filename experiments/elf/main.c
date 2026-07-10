#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <elf.h>

Elf64_Shdr *get_section(Elf64_Ehdr *eh, size_t section_number) {
    if (section_number > eh->e_shnum) {
        return nullptr;
    }

    return ((void *)eh) + eh->e_shoff + section_number * eh->e_shentsize;
}

void *get_section_content(Elf64_Ehdr *eh, Elf64_Shdr *sh) {
    return (void *)eh + sh->sh_offset;
}

const char *get_section_name_string(Elf64_Ehdr *eh, size_t index) {
    if (index == 0)
        return "";
    auto strndx = get_section(eh, eh->e_shstrndx);
    void *buf = get_section_content(eh, strndx);
    if (index > strndx->sh_size)
        return nullptr;
    return buf + index;
}

Elf64_Shdr *get_section_by_name(Elf64_Ehdr *eh, const char *name) {
    for (int i = 0; i < eh->e_shnum; i++) {
        auto s = get_section(eh, i);
        if (strcmp(get_section_name_string(eh, s->sh_name), name) == 0) {
            return s;
        }
    }
    return nullptr;
}

const char *get_string(Elf64_Ehdr *eh, size_t index) {
    if (index == 0)
        return "";
    auto strndx = get_section_by_name(eh, ".strtab");
    void *buf = get_section_content(eh, strndx);
    if (index > strndx->sh_size)
        return nullptr;
    return buf + index;
}

Elf64_Sym *get_symbol(Elf64_Ehdr *eh, size_t symbol_number) {
    auto symtab = get_section_by_name(eh, ".symtab");
    size_t symbol_count = symtab->sh_size / sizeof(Elf64_Sym);
    if (symbol_number > symbol_count) {
        return nullptr;
    }
    Elf64_Sym *sym = get_section_content(eh, symtab);
    return &sym[symbol_number];
}

size_t sh_count(Elf64_Shdr *sh) {
    if (sh->sh_entsize == 0)
        return 0;
    return sh->sh_size / sh->sh_entsize;
}

const char *relocation_name(int relocation_type) {
    switch (relocation_type) {
        case R_X86_64_NONE: return "none";
        case R_X86_64_64: return "64";
        case R_X86_64_PC32: return "pc32";
        case R_X86_64_GOT32: return "got32";
        case R_X86_64_PLT32: return "plt32";
        case R_X86_64_32S: return "32s";
        default: return "other/unknown";
    }
}

/* -------------------------------------------------------- */
void *open_module_file(const char *name, long *buffer_len) {
    FILE *module = fopen(name, "rb");
    if (!module) {
        perror("fopen");
        goto err;
    }

    if (fseek(module, 0, SEEK_END)) {
        perror("fseek");
        goto err_open;
    }
    long length = ftell(module);
    if (length <= 0) {
        perror("ftell");
        goto err_open;
    }
    void *buffer = calloc(1, length);
    if (!buffer) {
        perror("malloc");
        goto err_open;
    }
    rewind(module);
    if (fread(buffer, 1, length, module) != (size_t)length) {
        perror("fread");
        goto err_open;
    }
    fclose(module);

    *buffer_len = length;
    return buffer;

err_open:
    fclose(module);
err:
    return nullptr;
}
/* -------------------------------------------------------- */


int main() {
    long buffer_len;
    Elf64_Ehdr *eh = open_module_file("module.o", &buffer_len);
    if (!eh) {
        fprintf(stderr, "failed to open module file\n");
        exit(1);
    }

    auto s = get_section_by_name(eh, ".text");
    printf("%s\n", get_section_name_string(eh, s->sh_name));

    for (int i = 0; i < eh->e_shnum; i++) {
        auto s = get_section(eh, i);
        if (!(s->sh_flags & SHF_ALLOC))
            continue;
        printf("offset: %#lx,\tsize: %#lx,\t name: \"%s\"\n", s->sh_offset, s->sh_size, get_section_name_string(eh, s->sh_name));
    }

    for (int i = 0;; i++) {
        auto sym = get_symbol(eh, i);
        if (!sym) break;
        if (!sym->st_name) continue;

        printf("symbol (%i): \"%s\"\n", i, get_string(eh, sym->st_name));
    }

    auto rela_t = get_section_by_name(eh, ".rela.text");
    printf(".rela.text section: size: %lu, entsize: %lu\n", rela_t->sh_size, rela_t->sh_entsize);
    auto rela_d = get_section_by_name(eh, ".rela.data");
    printf(".rela.data section: size: %lu, entsize: %lu\n", rela_d->sh_size, rela_d->sh_entsize);

    printf("rel: %zu, rela: %zu\n", sizeof(Elf64_Rel), sizeof(Elf64_Rela));

    Elf64_Rela *r_t = get_section_content(eh, rela_t);
    printf("offset type sym addend (name)\n");
    for (size_t i = 0; i < sh_count(rela_t); i++) {
        size_t sym_index = ELF64_R_SYM(r_t[i].r_info);
        auto sym = get_symbol(eh, sym_index);

        printf("%lx %s %lx %lx (%s)\n",
                r_t[i].r_offset,
                relocation_name(ELF64_R_TYPE(r_t[i].r_info)),
                sym_index,
                r_t[i].r_addend,
                get_string(eh, sym->st_name)
        );
    }

    free(eh);
}
