#include <assert.h>
#include <elf.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/cdefs.h>
#include <errno.h>
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
elf_md *main_md;
void __nc_init();

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

	const Elf_Sym *lib_sym = nullptr;
	elf_md *owner = nullptr;
	if (main_md) {
		lib_sym = elf_find_dynsym(main_md, sym_name);
		if (lib_sym && lib_sym->st_shndx != SHN_UNDEF && lib_sym->st_value) {
			owner = main_md;
		} else {
			lib_sym = nullptr;
		}
	}
	if (!lib_sym && lib_md) {
		lib_sym = elf_find_dynsym(lib_md, sym_name);
		if (lib_sym && lib_sym->st_shndx != SHN_UNDEF && lib_sym->st_value) {
			owner = lib_md;
		} else {
			lib_sym = nullptr;
		}
	}
	if (!lib_sym || !owner) {
		DBG("Could not resolve '%s' - abort\n", sym_name);
		exit(1);
	}

	*got_entry = (Elf_Addr)owner->image + lib_sym->st_value;
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

	// actually load the library into virtual memory properly
	size_t map_size = lib_needed_virtual_size - lib_base;
	void *lib_load = mmap(nullptr, map_size, PROT_READ | PROT_WRITE,
		MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (lib_load == MAP_FAILED) {
		perror("mmap");
		return nullptr;
	}
	uintptr_t load_bias = (uintptr_t)lib_load - lib_base;
	lib->mmap = lib_load;
	lib->mmap_size = map_size;
	lib->header = PTR_ADD((void *)load_bias, 0);
	lib->image = (void *)load_bias;

	for (int i = 0; i < lib->imm_header->e_phnum; i++) {
		if (p[i].p_type != PT_LOAD)
			continue;
		void *seg = PTR_ADD(lib->image, p[i].p_vaddr);
		memcpy(seg, PTR_ADD(lib->buffer, p[i].p_offset), p[i].p_filesz);
		if (p[i].p_memsz > p[i].p_filesz) {
			memset(
				PTR_ADD(seg, p[i].p_filesz), 0, p[i].p_memsz - p[i].p_filesz);
		}
	}

	// get some useful things from the DYNAMIC section
	const Elf_Dyn *lib_dyn_jrel = elf_find_dyn(lib, DT_JMPREL);
	const Elf_Dyn *lib_dyn_jrelsz = elf_find_dyn(lib, DT_PLTRELSZ);
	const Elf_Dyn *lib_dyn_drel = elf_find_dyn(lib, DT_RELA);
	const Elf_Dyn *lib_dyn_drelsz = elf_find_dyn(lib, DT_RELASZ);
	const Elf_Dyn *lib_dyn_sym = elf_find_dyn(lib, DT_SYMTAB);
	const Elf_Dyn *lib_dyn_str = elf_find_dyn(lib, DT_STRTAB);
	const Elf_Dyn *lib_dyn_got = elf_find_dyn(lib, DT_PLTGOT);

	if (!lib_dyn_sym || !lib_dyn_str) {
		DBG("missing DT_SYMTAB/DT_STRTAB\n");
		return nullptr;
	}

	Elf_Rela *lib_jrel
		= lib_dyn_jrel ? PTR_ADD(lib->image, lib_dyn_jrel->d_un.d_ptr) : nullptr;
	Elf_Rela *lib_drel
		= lib_dyn_drel ? PTR_ADD(lib->image, lib_dyn_drel->d_un.d_ptr) : nullptr;
	Elf_Sym *lib_sym = PTR_ADD(lib->image, lib_dyn_sym->d_un.d_ptr);
	char *lib_str = PTR_ADD(lib->image, lib_dyn_str->d_un.d_ptr);
	Elf_Addr *lib_got
		= lib_dyn_got ? PTR_ADD(lib->image, lib_dyn_got->d_un.d_ptr) : nullptr;
	size_t lib_jrelsz = lib_dyn_jrelsz ? lib_dyn_jrelsz->d_un.d_val : 0;
	size_t lib_drelsz = lib_dyn_drelsz ? lib_dyn_drelsz->d_un.d_val : 0;
	int lib_jrelcnt = lib_jrelsz / sizeof(Elf_Rela);
	int lib_drelcnt = lib_drelsz / sizeof(Elf_Rela);


	// set GOT[1] and GOT[2]
	if (lib_got) {
		lib_got[1] = (Elf_Addr)lib;
		lib_got[2] = (Elf_Addr)elf_lazy_resolve_stub;
	}

	// Do what we can with PLT relocations
	for (int i = 0; i < lib_jrelcnt; i++) {
		Elf_Rela *rel = lib_jrel + i;
		int type = ELF64_R_TYPE(rel->r_info);
		assert(type == R_X86_64_JUMP_SLOT);

		int symix = ELF64_R_SYM(rel->r_info);
		Elf_Sym *sym = lib_sym + symix;

		Elf_Addr *got_entry = PTR_ADD(lib->image, rel->r_offset);
		if (sym->st_value) {
			*got_entry = (Elf_Addr)lib->image + sym->st_value;
		} else {
			// if we don't have a symbol, redirect the GOT entry placed
			// by the linker to point at the actual entry in the PLT
			// so that we actually make it to the runtime linker.
			//
			// This might be the correct behavior for all symbols, but
			// for now I'm going to eager load the ones we really have.
			*got_entry = *got_entry + (Elf_Addr)lib->image;
		}
	}

	// Resolve GOT relocations
	for (int i = 0; i < lib_drelcnt; i++) {
		Elf_Rela *rel = lib_drel + i;
		int type = ELF64_R_TYPE(rel->r_info);

		int symix = ELF64_R_SYM(rel->r_info);
		Elf_Sym *sym = lib_sym + symix;

		Elf_Addr *got_entry = PTR_ADD(lib->image, rel->r_offset);
		switch (type) {
		case R_X86_64_RELATIVE:
			*got_entry = (Elf_Addr)lib->image + rel->r_addend;
			break;
		case R_X86_64_GLOB_DAT:
		case R_X86_64_64: {
			Elf_Addr value = 0;
			if (sym->st_value) {
				value = (Elf_Addr)lib->image + sym->st_value;
			} else {
				char *sym_name = lib_str + sym->st_name;
			if (!lib_md && !main_md) {
				DBG("unable to resolve data symbol %s -- abort\n", sym_name);
				exit(1);
			}

			const Elf_Sym *lib_sym = nullptr;
			elf_md *owner = nullptr;
			if (main_md) {
				lib_sym = elf_find_dynsym(main_md, sym_name);
				if (lib_sym && lib_sym->st_shndx != SHN_UNDEF && lib_sym->st_value) {
					owner = main_md;
				} else {
					lib_sym = nullptr;
				}
			}
			if (!lib_sym && lib_md) {
				lib_sym = elf_find_dynsym(lib_md, sym_name);
				if (lib_sym && lib_sym->st_shndx != SHN_UNDEF && lib_sym->st_value) {
					owner = lib_md;
				} else {
					lib_sym = nullptr;
				}
			}
			if (!lib_sym || !owner) {
				DBG("unable to resolve data symbol %s -- abort\n", sym_name);
				exit(1);
			}

			value = (Elf_Addr)owner->image + lib_sym->st_value;
			}
			*got_entry = value + rel->r_addend;
			break;
		}
		default:
			DBG("unhandled relocation type %i\n", type);
			exit(1);
		}
	}

	return lib_load;
}

void run_dyn_ld(int argc, char **argv, char **envp) {
	// FIXME relocate self
	// FIXME multiple libraries

	// we want to:
	// take a "libc" .so dynamic library and load it into memory
	// take a "main" dynamic executable and load + link it to libc

	elf_md *lib = elf_open("/lib/libc.so");
	if (!lib) {
		DBG("unable to load libc.so\n");
		exit(1);
	}
	elf_md *main = nullptr;
	const char *main_path = nullptr;
	if (argc > 1) {
		main_path = argv[1];
	} else if (argc > 0) {
		main_path = argv[0];
	}
	if (main_path) {
		main = elf_open(main_path);
		if (!main && !strchr(main_path, '/')) {
			char full_path[256];
			size_t len = strlen(main_path);
			if (len + 5 < sizeof(full_path)) {
				memcpy(full_path, "/bin/", 5);
				memcpy(full_path + 5, main_path, len + 1);
				main = elf_open(full_path);
			}
		}
	} else {
		main = elf_parse((Elf_Ehdr *)0x400000, 0);
	}
	if (!main) {
		DBG("unable to load main executable\n");
		exit(1);
	}

	lib_md = lib; // the "global symbol table"
	main_md = main;

	elf_print(lib);
	elf_print(main);

	elf_dyld_load(lib);
	elf_dyld_load(main);

	void (*_start)(int, char **, char **);
	_start = (void (*)(int, char **, char **))(
		main->image + main->imm_header->e_entry);
	_start(argc, argv, envp);
}

// dlopen, dlsym, etc

int main(int argc, char **argv, char **envp) {
	run_dyn_ld(argc, argv, envp);
}

int _start(int argc, char **argv, char **envp) {
	__nc_init();
	_exit(main(argc, argv, envp));
}
