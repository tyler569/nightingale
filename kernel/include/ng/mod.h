#pragma once

#include <elf.h>
#include <list.h>
#include <rbtree.h>
#include <sys/cdefs.h>

BEGIN_DECLS

enum modinit_status {
	MODINIT_SUCCESS,
	MODINIT_FAILURE,
};

struct mod;

struct modinfo {
	const char *name;
	int (*init)(struct mod *);
	void (*fini)(struct mod *);
};

struct mod {
	const char *name;
	struct modinfo *modinfo;
	elf_md *md;
	uintptr_t load_base;

	struct list deps;
	int refcnt;
	list_node node;
};

int load_mod(Elf_Ehdr *elf, size_t len);
int unload_mod(struct mod *mod); // not implemented

struct mod_sym {
	const struct mod *mod;
	const Elf_Sym *sym;
};

// Symbol node for rbtree-based symbol lookup
struct symbol_node {
	struct rbnode rb;           // RBTree node (must be first)
	uintptr_t address;          // Symbol address
	uintptr_t size;             // Symbol size
	const char *symbol_name;    // Pointer to ELF string table
	const struct mod *mod;      // Module reference (NULL for kernel)
};

struct mod_sym elf_find_symbol_by_address(uintptr_t address);
struct symbol_node *symbol_find_by_address(uintptr_t address);
void symbol_tree_init(void);

END_DECLS
