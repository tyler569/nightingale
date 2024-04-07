#include <assert.h>
#include <elf.h>
#include <ng/common.h>
#include <ng/fs.h>
#include <ng/vmm.h>
#include <stdio.h>
#include <string.h>

elf_md elf_ngk_md;

void elf_relo_resolve(elf_md *module, elf_md *main);

void elf_relocate(elf_md *e, Elf_Shdr *modify_section, Elf_Rela *rela) {
	size_t sym_index = ELF64_R_SYM(rela->r_info);
	Elf_Sym *sym = &e->mut_symbol_table[sym_index];
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
	case R_X86_64_NONE:
		break;
	case R_X86_64_64:
		*(uint64_t *)relocation = S + A;
		break;
	case R_X86_64_32:
		*(uint32_t *)relocation = S + A;
		break;
	case R_X86_64_32S:
		*(int32_t *)relocation = S + A;
		break;
	case R_X86_64_PC32: // FALLTHROUGH
	case R_X86_64_PLT32:
		*(uint32_t *)relocation = S + A - P;
		break;
	default:
		printf("invalid relocation type: %i\n", rel_type);
	}
}

elf_md *elf_relo_load(elf_md *relo) {
	// get needed virtual allocation size (file size + sum of all common
	// symbol sizes)
	size_t file_size = relo->file_size;
	size_t relo_needed_virtual_size = relo->file_size;
	size_t relo_common_size = 0;
	assert(relo_needed_virtual_size > 0);

	if (relo->imm_header->e_type != ET_REL)
		return NULL;
	if (!relo->symbol_table)
		return NULL;
	if (!relo->string_table)
		return NULL;

	const Elf_Shdr *o_bss = elf_find_section(relo, ".bss");

	// total size of common symbols (tenative definitions)
	// some of these will not be used, since they'll resolve to external
	// symbols, but I have no reasonable way to allocate more space after
	// that linking process happens.
	// I suppose it would be possible to do another round of this then, but
	// we'll consider that if it becomes a problem.
	for (int i = 0; i < relo->symbol_count; i++) {
		const Elf_Sym *sym = relo->symbol_table + i;
		if (sym->st_shndx == SHN_COMMON) {
			// This math must match the bss symbol placement below
			relo_common_size = ROUND_UP(relo_common_size, sym->st_value);
			relo_common_size += sym->st_size;
		}
	}
	if (o_bss)
		relo_common_size += o_bss->sh_size;
	relo_needed_virtual_size += relo_common_size;

	void *relo_load = malloc(relo_needed_virtual_size);
	relo->bss_base = PTR_ADD(relo_load, relo->file_size);
	uintptr_t bss_base = (uintptr_t)relo->bss_base;
	uintptr_t comm_base = bss_base + (o_bss ? o_bss->sh_size : 0);
	uintptr_t comm_cursor = comm_base;

	memcpy(relo_load, relo->buffer, relo->file_size);
	memset(relo->bss_base, 0, relo_common_size);

	relo->file_size = file_size;
	relo->image = relo_load;
	relo->header = relo_load;
	relo->mut_section_headers = PTR_ADD(relo_load, relo->header->e_shoff);
	Elf_Shdr *mut_symtab = elf_find_section_mut(relo, ".symtab");
	relo->mut_symbol_table = PTR_ADD(relo_load, mut_symtab->sh_offset);
	relo->mmap = relo_load;
	relo->mmap_size = relo_needed_virtual_size;

	// Set shdr->sh_addr to their loaded addresses.
	for (int i = 0; i < relo->section_header_count; i++) {
		Elf_Shdr *shdr = &relo->mut_section_headers[i];
		shdr->sh_addr = shdr->sh_offset + (uintptr_t)relo->image;
	}

	Elf_Shdr *bss = elf_find_section_mut(relo, ".bss");
	if (bss)
		bss->sh_addr = (uintptr_t)relo->bss_base;
	size_t bss_shndx = bss ? bss - relo->mut_section_headers : -1;

	// Place bss symbols' st_values in bss region, common symbol' st_values
	// off the end of the bss_region, and absolutize the sh_addr
	// of all symbols.
	for (int i = 0; i < relo->symbol_count; i++) {
		Elf_Sym *sym = relo->mut_symbol_table + i;
		size_t value = sym->st_value;
		if (sym->st_shndx == SHN_COMMON) {
			// In relocatable files, st_value holds alignment constraints
			// for a symbol whose section index is SHN_COMMON.
			//
			comm_cursor = ROUND_UP(comm_cursor, value);
			sym->st_value = comm_cursor;
			comm_cursor += sym->st_size;
		} else if (sym->st_shndx == bss_shndx) {
			// In relocatable files, st_value holds a section offset for a
			// defined symbol. That is, st_value is an offset from the
			// beginning of the section that st_shndx identifies.
			sym->st_value = bss_base + sym->st_value;
		} else if (sym->st_shndx != SHN_UNDEF) {
			Elf_Shdr *shdr = &relo->mut_section_headers[sym->st_shndx];
			sym->st_value += shdr->sh_addr;
		}
	}

	elf_relo_resolve(relo, &elf_ngk_md);

	for (int i = 0; i < relo->imm_header->e_shnum; i++) {
		Elf_Shdr *sec = relo->mut_section_headers + i;
		if (sec->sh_type != SHT_RELA)
			continue;

		Elf_Shdr *modify_section = relo->mut_section_headers + sec->sh_info;
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
		Elf_Sym *sym = &module->mut_symbol_table[i];
		int type = ELF_ST_TYPE(sym->st_info);
		if (type == STT_FILE)
			continue;
		if (sym->st_shndx == SHN_UNDEF || sym->st_shndx == SHN_COMMON) {
			const char *name = elf_symbol_name(module, sym);
			if (!name || !name[0])
				continue;
			const Elf_Sym *psym = elf_find_symbol(main, name);
			if (!psym)
				continue;
			sym->st_value = psym->st_value;
			sym->st_info = psym->st_info;
			sym->st_shndx = SHN_ABS;
			// printf("resolved '%s' -> %p\n", name, sym->st_value);
		}
	}
}

void *elf_sym_addr(const elf_md *e, const Elf_Sym *sym) {
	// if (sym->st_shndx == SHN_COMMON) {
	//     return (char *)e->bss_base + sym->st_value;
	// }
	// const Elf_Shdr *section;
	// if (e->mut_section_headers) {
	//     section = &e->mut_section_headers[sym->st_shndx];
	// } else {
	//     section = &e->section_headers[sym->st_shndx];
	// }
	return (void *)sym->st_value; // (char *)section->sh_addr + sym->st_value;
}

void limine_load_kernel_elf(void *ptr, size_t len) {
	elf_md *tmp = elf_parse(ptr, len);
	elf_ngk_md = *tmp;
	free(tmp);
}

elf_md *elf_mod_load(struct inode *elf_inode) {
	if (elf_inode->type != FT_NORMAL)
		return NULL;
	elf_md *mod = elf_parse(elf_inode->data, elf_inode->len);

	mod = elf_relo_load(mod);
	if (!mod)
		return NULL;

	for (int i = 0; i < mod->symbol_count; i++) {
		const Elf_Sym *sym = mod->mut_symbol_table + i;
		if (sym->st_shndx == SHN_UNDEF) {
			const char *name = elf_symbol_name(mod, sym);
			if (!name || !name[0])
				continue;
			printf("warning: symbol '%s' not found\n", name);
		}
	}

	return mod;
}
