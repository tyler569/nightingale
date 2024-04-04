#include <assert.h>
#include <elf.h>
#include <errno.h>
#include <ng/common.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void elf_print(const elf_md *e) {
	printf("elf @ (imm:%p) (mut:%p)\n", e->buffer, e->image);
}

static const char elf64_header_example[8] = {
	0x7F,
	'E',
	'L',
	'F',
	ELF64,
	ELFLE,
	ELFVERSION,
	ELFABI,
};

#define VERIFY_DEPTH 8

int elf_verify(const Elf_Ehdr *elf) {
	if (memcmp(elf, elf64_header_example, VERIFY_DEPTH) == 0) {
		return 64;
	} else {
		printf("\ntried to load: [ ");
		for (int i = 0; i < 16; i++) {
			printf("%02hhx ", ((char *)elf)[i]);
		}
		printf("]\n");
		return 0;
	}
}

/*
 * Always returns the first matching header, if you need multiple (i.e. all
 * the PT_LOADs, just iterate yourself.)
 */
const Elf_Phdr *elf_find_phdr(const elf_md *e, int p_type) {
	if (!e->program_headers)
		return NULL;

	for (int i = 0; i < e->imm_header->e_phnum; i++) {
		const Elf_Phdr *hdr = e->program_headers + i;
		if (hdr->p_type == p_type)
			return hdr;
	}
	return NULL;
}

const Elf_Dyn *elf_find_dyn(const elf_md *e, int d_tag) {
	const Elf_Dyn *d = e->dynamic_table;
	if (!d)
		return NULL;

	for (; d->d_tag != DT_NULL; d++) {
		if (d->d_tag == d_tag)
			return d;
	}
	return NULL;
}

const Elf_Shdr *elf_find_section(const elf_md *e, const char *name) {
	const Elf_Shdr *shdr_table = e->section_headers;
	if (!shdr_table)
		return NULL;

	for (int i = 0; i < e->section_header_count; i++) {
		const Elf_Shdr *shdr = shdr_table + i;
		const char *sh_name = e->section_header_string_table + shdr->sh_name;
		if (strcmp(sh_name, name) == 0)
			return shdr;
	}
	return NULL;
}

Elf_Shdr *elf_find_section_mut(const elf_md *e, const char *name) {
	Elf_Shdr *shdr_table = e->mut_section_headers;
	if (!shdr_table)
		return NULL;

	for (int i = 0; i < e->section_header_count; i++) {
		Elf_Shdr *shdr = shdr_table + i;
		const char *sh_name = e->section_header_string_table + shdr->sh_name;
		if (strcmp(sh_name, name) == 0)
			return shdr;
	}
	return NULL;
}

const char *elf_symbol_name(const elf_md *e, const Elf_Sym *sym) {
	return &e->string_table[sym->st_name];
}

const Elf_Sym *elf_find_symbol(const elf_md *e, const char *name) {
	const Elf_Sym *sym_tab;
	if (e->imm_header) {
		sym_tab = e->mut_symbol_table;
	} else {
		sym_tab = e->symbol_table;
	}
	if (!sym_tab)
		return 0;

	for (int i = 0; i < e->symbol_count; i++) {
		const Elf_Sym *symbol = sym_tab + i;
		const char *symbol_name = elf_symbol_name(e, symbol);
		if (strcmp(name, symbol_name) == 0)
			return symbol;
	}
	return NULL;
}

const Elf_Sym *elf_symbol_by_address(const elf_md *e, uintptr_t address) {
	size_t nsymbols = e->symbol_count;
	const Elf_Sym *symtab;
	if (e->mut_symbol_table) {
		symtab = e->mut_symbol_table;
	} else {
		symtab = e->symbol_table;
	}

	uintptr_t addr_match = 0;
	const Elf_Sym *best_match = NULL;

	for (size_t i = 0; i < nsymbols; i++) {
		const Elf_Sym *sym = symtab + i;

		if (sym->st_name == 0)
			continue;
		if (sym->st_value > address)
			continue;

		if (sym->st_value > addr_match) {
			best_match = sym;
			addr_match = sym->st_value;
		}
	}

	return best_match;
}

const Elf_Sym *elf_find_dynsym(const elf_md *e, const char *name) {
	// todo: dynsym hash table
	for (int i = 0; i < e->dynsym_count; i++) {
		const Elf_Sym *sym = e->dynsym + i;
		const char *sym_name = elf_symbol_name(e, sym);
		if (strcmp(name, sym_name) == 0)
			return sym;
	}
	return NULL;
}

elf_md *elf_parse(const void *buffer, size_t buffer_len) {
	if (!elf_verify(buffer))
		return NULL;
	elf_md *e = calloc(1, sizeof(*e));
	const Elf_Ehdr *elf = buffer;

	e->imm_header = elf;
	e->buffer = buffer;
	e->file_size = buffer_len;

	if (elf->e_shnum > 0) {
		e->section_headers = PTR_ADD(buffer, elf->e_shoff);
		e->section_header_count = elf->e_shnum;
		const Elf_Shdr *shstrtab = e->section_headers + elf->e_shstrndx;
		e->section_header_string_table = PTR_ADD(buffer, shstrtab->sh_offset);
	}

	const Elf_Shdr *strtab = elf_find_section(e, ".strtab");
	const Elf_Shdr *symtab = elf_find_section(e, ".symtab");

	if (strtab)
		e->string_table = PTR_ADD(buffer, strtab->sh_offset);

	if (symtab) {
		e->symbol_table = PTR_ADD(buffer, symtab->sh_offset);
		e->symbol_count = symtab->sh_size / symtab->sh_entsize;
	}

	if (elf->e_phnum > 0) {
		e->program_headers = PTR_ADD(buffer, elf->e_phoff);
	}

	const Elf_Phdr *dynamic_phdr = elf_find_phdr(e, PT_DYNAMIC);
	if (dynamic_phdr) {
		e->dynamic_table = PTR_ADD(buffer, dynamic_phdr->p_offset);
		e->dynamic_count = dynamic_phdr->p_filesz / sizeof(Elf_Dyn);
	}

	const Elf_Shdr *dynsym_section = elf_find_section(e, ".dynsym");
	if (dynsym_section) {
		e->dynsym = PTR_ADD(buffer, dynsym_section->sh_offset);
		e->dynsym_count = dynsym_section->sh_size / dynsym_section->sh_entsize;
	}

	return e;
}

elf_md *clone_elf_md(elf_md *old) {
	elf_md *e = calloc(1, sizeof(*e));
	*e = *old;
	return e;
}

/* Straight from the ELF spec */
unsigned long elf_hash(const unsigned char *name) {
	unsigned long h = 0, g;
	while (*name) {
		h = (h << 4) + *name++;
		if ((g = h & 0xf0000000))
			h ^= g >> 24;
		h &= ~g;
	}
	return h;
}
