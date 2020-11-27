#include <basic.h>
#include <assert.h>
#include <elf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PTR_ADD(p, off) (void *)(((char *)p) + off)

void elf_print(elf_md *e) {
    printf("elf @ %p\n", e->mem);
}

/*
 * Always returns the first matching header, if you need multiple (i.e. all
 * the PT_LOADs, just iterate yourself.)
 */
Elf_Phdr *elf_find_phdr(elf_md *e, int p_type) {
    if (!e->program_headers) return NULL;

    for (int i = 0; i < e->image->e_phnum; i++) {
        Elf_Phdr *hdr = e->program_headers + i;
        if (hdr->p_type == p_type) return hdr;
    }
    return NULL;
}

Elf_Dyn *elf_find_dyn(elf_md *e, int d_tag) {
    Elf_Dyn *d = e->dynamic_table;
    if (!d) return NULL;

    for (; d->d_tag != DT_NULL; d++) {
        if (d->d_tag == d_tag) return d;
    }
    return NULL;
}

Elf_Shdr *elf_find_section(elf_md *e, const char *name) {
    Elf_Shdr *shdr_table = e->section_headers;
    if (!shdr_table) return NULL;

    for (int i = 0; i < e->section_header_count; i++) {
        Elf_Shdr *shdr = shdr_table + i;
        const char *sh_name = e->shdr_string_table + shdr->sh_name;
        if (strcmp(sh_name, name) == 0) return shdr;
    }
    return NULL;
}

Elf_Sym *elf_find_symbol(elf_md *e, const char *name) {
    // TODO: symbol hash table lookups
    Elf_Sym *sym_tab = e->symbol_table;
    if (!sym_tab) return 0;

    int symbol_table_count = e->symbol_table_section->sh_size / sizeof(Elf_Sym);

    for (int i = 0; i < symbol_table_count; i++) {
        Elf_Sym *symbol = sym_tab + i;
        const char *symbol_name = e->string_table + symbol->st_name;
        if (strcmp(name, symbol_name) == 0) { return symbol; }
    }
    return NULL;
}

elf_md *elf_parse(void *memory) {
    elf_md *e = calloc(1, sizeof(*e));
    Elf_Ehdr *elf = memory;

    e->image = elf;
    e->mem = memory;

    e->section_header_count = elf->e_shnum;
    if (elf->e_shnum > 0) {
        e->section_headers = PTR_ADD(memory, elf->e_shoff);
        e->shdr_string_table_section = e->section_headers + elf->e_shstrndx;
        e->shdr_string_table =
            PTR_ADD(memory, e->shdr_string_table_section->sh_offset);
    }

    e->string_table_section = elf_find_section(e, ".strtab");
    e->symbol_table_section = elf_find_section(e, ".symtab");

    if (e->string_table_section) {
        e->string_table = PTR_ADD(memory, e->string_table_section->sh_offset);
    }

    if (e->symbol_table_section) {
        e->symbol_table = PTR_ADD(memory, e->symbol_table_section->sh_offset);
        e->symbol_count =
            e->symbol_table_section->sh_size /
            e->symbol_table_section->sh_entsize;
    }

    if (elf->e_phnum > 0) {
        e->program_headers = PTR_ADD(memory, elf->e_phoff);
    }

    Elf_Phdr *dynamic_phdr = elf_find_phdr(e, PT_DYNAMIC);
    if (dynamic_phdr) {
        e->dynamic_table = PTR_ADD(e->mem, dynamic_phdr->p_offset);
        e->dynamic_count = dynamic_phdr->p_filesz / sizeof(Elf_Dyn);
    }

    return e;
}

/* Straight from the ELF spec */
unsigned long elf_hash(const unsigned char *name) {
    unsigned long h = 0, g;
    while (*name) {
        h = (h << 4) + *name++;
        if ((g = h & 0xf0000000)) h ^= g >> 24;
        h &= ~g;
    }
    return h;
}

const char *elf_symbol_name(elf_md *e, Elf_Sym *sym) {
    return &e->string_table[sym->st_name];
}

#ifndef __kernel__
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

void fail(const char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}

elf_md *elf_open(const char *name) {
    int fd = open(name, O_RDONLY);
    if (fd < 0) fail("open");

    struct stat statbuf;
    fstat(fd, &statbuf);
    off_t len = statbuf.st_size;

    void *mem = mmap(NULL, len, PROT_READ, MAP_PRIVATE, fd, 0);
    elf_md *e = elf_parse(mem);
    e->file_size = len;
    return e;
}
#endif // ifndef __kernel__
