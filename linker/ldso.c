#include <assert.h>
#include <elf.h>
#include <fcntl.h>
#include <limits.h>
#include <ng/common.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// pltstub.S
void elf_lazy_resolve_stub();
elf_md *lib_md; // the "global symbol table"

#define DBG(...) printf(__VA_ARGS__)

void (*elf_lazy_resolve(elf_md *o, long rel_index))() {
	DBG("lazy resolving %li with elf %p -- ", rel_index, o);
	const Elf_Dyn *obj_dyn_rel = elf_find_dyn(o, DT_JMPREL);
	const Elf_Dyn *obj_dyn_sym = elf_find_dyn(o, DT_SYMTAB);
	const Elf_Dyn *obj_dyn_str = elf_find_dyn(o, DT_STRTAB);

	Elf_Rela *obj_rel = o->image + obj_dyn_rel->d_un.d_ptr;
	Elf_Sym *obj_sym = o->image + obj_dyn_sym->d_un.d_ptr;
	char *obj_str = o->image + obj_dyn_str->d_un.d_ptr;

	Elf_Rela *rel = obj_rel + rel_index;
	Elf_Addr *got_entry = o->image + rel->r_offset;

	int type = ELF64_R_TYPE(rel->r_info);
	assert(type == R_X86_64_JUMP_SLOT);

	int symix = ELF64_R_SYM(rel->r_info);
	Elf_Sym *sym = obj_sym + symix;
	char *sym_name = obj_str + sym->st_name;

	DBG("(%s)\n", sym_name);

	const Elf_Sym *lib_sym = elf_find_dynsym(lib_md, sym_name);
	if (!lib_sym) {
		DBG("Could not resolve '%s' - abort\n", sym_name);
		exit(1);
	}

	*got_entry = (Elf_Addr)lib_md->image + lib_sym->st_value;
	return (void (*)())*got_entry;
}

void *elf_dyld_load(elf_md *lib) {
	// get needed virtual allocation size - max(ph.vaddr + ph.memsz)
	size_t lib_needed_virtual_size = 0;
	uintptr_t lib_base = UINTPTR_MAX;
	const Elf_Phdr *p = lib->program_headers;
	for (int i = 0; i < lib->imm_header->e_phnum; i++) {
		if (p[i].p_type != PT_LOAD)
			continue;
		size_t max = p[i].p_vaddr + p[i].p_memsz;
		if (max > lib_needed_virtual_size)
			lib_needed_virtual_size = max;
		size_t base = p[i].p_vaddr;
		if (base < lib_base)
			lib_base = base;
	}

	void *load_request = (void *)lib_base;

	// actually load the library into virtual memory properly
	void *lib_load = mmap(load_request, lib_needed_virtual_size - lib_base,
		PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	lib->mmap = lib_load;
	lib->mmap_size = lib_needed_virtual_size - lib_base;
	lib->header = lib_load; // maybe -- not sure it has to load the ehdr

	if (lib_base != 0) {
		/*
		 * The ELF specified a base, meaning all virtual address lookups from
		 * the ELF data from here out are _already correct_. You only add
		 * this base if the ELF actually specified a base of 0.
		 */
		lib_load = (void *)0;
	}
	lib->image = lib_load;

	for (int i = 0; i < lib->imm_header->e_phnum; i++) {
		if (p[i].p_type != PT_LOAD)
			continue;
		memcpy(lib->image + p[i].p_vaddr, lib->buffer + p[i].p_offset,
			p[i].p_filesz);

		// memset the rest to 0 if filesz < memsz
	}

	// get some useful things from the DYNAMIC section
	const Elf_Dyn *lib_dyn_jrel = elf_find_dyn(lib, DT_JMPREL);
	const Elf_Dyn *lib_dyn_jrelsz = elf_find_dyn(lib, DT_PLTRELSZ);
	const Elf_Dyn *lib_dyn_drel = elf_find_dyn(lib, DT_RELA);
	const Elf_Dyn *lib_dyn_drelsz = elf_find_dyn(lib, DT_RELASZ);
	const Elf_Dyn *lib_dyn_sym = elf_find_dyn(lib, DT_SYMTAB);
	const Elf_Dyn *lib_dyn_str = elf_find_dyn(lib, DT_STRTAB);
	const Elf_Dyn *lib_dyn_got = elf_find_dyn(lib, DT_PLTGOT);

	Elf_Rela *lib_jrel = PTR_ADD(lib->image, lib_dyn_jrel->d_un.d_ptr);
	Elf_Rela *lib_drel = PTR_ADD(lib->image, lib_dyn_drel->d_un.d_ptr);
	Elf_Sym *lib_sym = PTR_ADD(lib->image, lib_dyn_sym->d_un.d_ptr);
	char *lib_str = PTR_ADD(lib->image, lib_dyn_str->d_un.d_ptr);
	Elf_Addr *lib_got = PTR_ADD(lib->image, lib_dyn_got->d_un.d_ptr);
	size_t lib_jrelsz = lib_dyn_jrelsz->d_un.d_val;
	size_t lib_drelsz = lib_dyn_drelsz->d_un.d_val;
	int lib_jrelcnt = lib_jrelsz / sizeof(Elf_Rela);
	int lib_drelcnt = lib_drelsz / sizeof(Elf_Rela);

	DBG("lib load base: %p\n", lib_load);
	DBG("lib dynsym:    + %p\n", (char *)lib_sym - (uintptr_t)lib_load);
	DBG("lib pltgot:    + %p\n", (char *)lib_got - (uintptr_t)lib_load);
	DBG("lib dynstr:    + %p\n", (char *)lib_str - (uintptr_t)lib_load);
	DBG("lib dynrel:    + %p\n", (char *)lib_jrel - (uintptr_t)lib_load);
	DBG("lib datrel:    + %p\n", (char *)lib_drel - (uintptr_t)lib_load);
	DBG("lib relsz:     %zu\n", lib_jrelsz);
	DBG("lib relcnt:    %i\n", lib_jrelcnt);
	DBG("lib drelcnt:   %i\n", lib_drelcnt);

	// take a look at the relocations we have
	for (int i = 0; i < lib_jrelcnt; i++) {
		Elf_Rela *rel = lib_jrel + i;
		int type = ELF64_R_TYPE(rel->r_info);

		int symix = ELF64_R_SYM(rel->r_info);
		Elf_Sym *sym = lib_sym + symix;

		char *sym_name = lib_str + sym->st_name;
		DBG("relocation: type: %i, symbol: '%s'\n", type, sym_name);

		DBG("  r_offset: %zx\n", rel->r_offset);
		DBG("  r_addend: %zi\n", rel->r_addend);
		DBG("  st_value: %zx\n", sym->st_value);
	}

	// set GOT[1] and GOT[2]
	lib_got[1] = (Elf_Addr)lib;
	lib_got[2] = (Elf_Addr)elf_lazy_resolve_stub;

	// Do what we can with PLT relocations
	for (int i = 0; i < lib_jrelcnt; i++) {
		Elf_Rela *rel = lib_jrel + i;
		int type = ELF64_R_TYPE(rel->r_info);
		assert(type == R_X86_64_JUMP_SLOT);

		int symix = ELF64_R_SYM(rel->r_info);
		Elf_Sym *sym = lib_sym + symix;

		Elf_Addr *got_entry = lib_load + rel->r_offset;
		if (sym->st_value) {
			*got_entry = (Elf_Addr)lib_load + sym->st_value;
		} else {
			// if we don't have a symbol, redirect the GOT entry placed
			// by the linker to point at the actual entry in the PLT
			// so that we actually make it to the runtime linker.
			//
			// This might be the correct behavior for all symbols, but
			// for now I'm going to eager load the ones we really have.
			*got_entry = *got_entry + (Elf_Addr)lib_load;
		}
	}

	// Resolve GOT relocations
	for (int i = 0; i < lib_drelcnt; i++) {
		Elf_Rela *rel = lib_drel + i;
		int type = ELF64_R_TYPE(rel->r_info);
		assert(type == R_X86_64_GLOB_DAT);

		int symix = ELF64_R_SYM(rel->r_info);
		Elf_Sym *sym = lib_sym + symix;

		Elf_Addr *got_entry = lib_load + rel->r_offset;
		if (sym->st_value) {
			*got_entry = (Elf_Addr)lib_load + sym->st_value;
		} else {
			char *sym_name = lib_str + sym->st_name;
			if (!lib_md) {
				DBG("unable to resolve data symbol %s -- abort\n", sym_name);
				exit(1);
			}

			const Elf_Sym *lib_sym = elf_find_dynsym(lib_md, sym_name);
			if (!lib_sym) {
				DBG("unable to resolve data symbol %s -- abort\n", sym_name);
				exit(1);
			}

			*got_entry = (Elf_Addr)lib_md->image + lib_sym->st_value;
		}
	}

	return lib_load;
}

void run_dyn_ld(int argc, char **argv, char **envp) {
	DBG("_DYNAMIC: %p\n", _DYNAMIC);
	DBG("GOT:      %p\n", (void *)_GLOBAL_OFFSET_TABLE_[0]);

	// FIXME relocate self
	// FIXME multiple libraries

	// we want to:
	// take a "libc" .so dynamic library and load it into memory
	// take a "main" dynamic executable and load + link it to libc

	elf_md *lib = elf_open("/usr/lib/libc.so");
	elf_md *main = elf_parse((Elf_Ehdr *)0x400000, 0);

	lib_md = lib; // the "global symbol table"

	elf_print(lib);
	elf_print(main);

	elf_dyld_load(lib);
	elf_dyld_load(main);

	void (*_start)(int, char **, char **);
	_start = (void (*)())(main->image + main->imm_header->e_entry);
	_start(argc, argv, envp);
}

// dlopen, dlsym, etc

int main(int argc, char **argv, char **envp) {
	run_dyn_ld(argc, argv, envp);
}

int _start(int argc, char **argv, char **envp) {
	_exit(main(argc, argv, envp));
}
