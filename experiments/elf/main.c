#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <elf.h>

Elf64_Shdr *get_section(Elf64_Ehdr *ehdr, size_t section_number) {
    if (section_number > ehdr->e_shnum) {
        return nullptr;
    }

    return ((void *)ehdr) + ehdr->e_shoff + section_number * ehdr->e_shentsize;
}

const char *get_section_name_string(Elf64_Ehdr *ehdr, size_t index) {
    if (index == 0)
        return "";
    auto strndx = get_section(ehdr, ehdr->e_shstrndx);
    void *buf = (void *)ehdr + strndx->sh_offset;
    if (index > strndx->sh_size)
        return nullptr;
    return buf + index;
}

Elf64_Shdr *get_section_by_name(Elf64_Ehdr *ehdr, const char *name) {
    for (int i = 0; i < ehdr->e_shnum; i++) { // no sections, shnum will be 0
        auto s = get_section(ehdr, i);
        if (strcmp(get_section_name_string(ehdr, s->sh_name), name) == 0) { // no strings, get_string will return "" or nullptr and never strcmp
            return s;
        }
    }
    return nullptr;
}

const char *get_string(Elf64_Ehdr *ehdr, size_t index) {
    if (index == 0)
        return "";
    auto strndx = get_section_by_name(ehdr, ".strtab");
    void *buf = (void *)ehdr + strndx->sh_offset;
    if (index > strndx->sh_size)
        return nullptr;
    return buf + index;
}

Elf64_Sym *get_symbol(Elf64_Ehdr *ehdr, size_t symbol_number) {
    auto symtab = get_section_by_name(ehdr, ".symtab");
    size_t symbol_count = symtab->sh_size / sizeof(Elf64_Sym);
    if (symbol_number > symbol_count) {
        return nullptr;
    }
    Elf64_Sym *sym = (void *)ehdr + symtab->sh_offset;
    return &sym[symbol_number];
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


    free(eh);
}
