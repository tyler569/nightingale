#include <elf.h>
#include <list.h>
#include <ng/fs.h>
#include <ng/mod.h>
#include <ng/slab.h>
#include <ng/sync.h>
#include <rbtree.h>
#include <stdlib.h>
#include <string.h>

struct list loaded_mods = LIST_INIT(loaded_mods);

// Global symbol tree for O(log n) address lookup
static struct rbtree symbol_tree;
static struct slab_cache symbol_cache;
static mutex_t symbol_tree_lock;

// Comparator function for symbol addresses
static int symbol_addr_compare(void *a, void *b) {
	uintptr_t addr_a = (uintptr_t)a;
	uintptr_t addr_b = (uintptr_t)b;

	if (addr_a < addr_b)
		return -1;
	if (addr_a > addr_b)
		return 1;
	return 0;
}

// Extract address key from symbol node
static void *symbol_get_key(struct rbnode *node) {
	struct symbol_node *snode = (struct symbol_node *)node;
	return (void *)snode->address;
}

// Initialize symbol tree with kernel symbols
void symbol_tree_init(void) {
	// Initialize mutex and slab cache
	mutex_init(&symbol_tree_lock);
	slab_cache_init(&symbol_cache, sizeof(struct symbol_node));

	// Initialize rbtree
	symbol_tree.root = NULL;
	symbol_tree.compare = symbol_addr_compare;
	symbol_tree.get_key = symbol_get_key;

	// Get kernel symbol table
	const Elf_Sym *symtab = elf_ngk_md.mut_symbol_table
		? elf_ngk_md.mut_symbol_table
		: elf_ngk_md.symbol_table;

	if (!symtab)
		return;

	// Populate tree with kernel symbols
	for (size_t i = 0; i < elf_ngk_md.symbol_count; i++) {
		const Elf_Sym *sym = &symtab[i];

		// Skip symbols without names or valid addresses
		if (sym->st_name == 0 || sym->st_value == 0)
			continue;

		// Allocate node from slab
		struct symbol_node *node = slab_alloc(&symbol_cache);
		if (!node)
			continue;

		// Set node fields
		node->symbol_name = elf_symbol_name(&elf_ngk_md, sym);
		node->address = sym->st_value;
		node->size = sym->st_size;
		node->mod = NULL; // NULL indicates kernel symbol

		// Insert into tree
		rbtree_insert(&symbol_tree, &node->rb);
	}
}

// Fast O(log n) symbol lookup using rbtree
struct mod_sym elf_find_symbol_by_address(uintptr_t address) {
	mutex_lock(&symbol_tree_lock);

	// Search for largest address <= query address
	struct rbnode *node = rbtree_search_le(&symbol_tree, (void *)address);

	mutex_unlock(&symbol_tree_lock);

	if (!node)
		return (struct mod_sym) { NULL, NULL };

	struct symbol_node *snode = (struct symbol_node *)node;

	// For compatibility, need to construct an Elf_Sym
	// Since we're just pointing to existing data, we can return the original sym
	// But we need to find it... Actually, we should change the API
	// For now, let's just return NULL for sym and rely on the new API
	// The backtrace code will be updated to use symbol_node directly
	return (struct mod_sym) { snode->mod, NULL };
}

// New API: returns symbol_node directly (more efficient)
struct symbol_node *symbol_find_by_address(uintptr_t address) {
	mutex_lock(&symbol_tree_lock);
	struct rbnode *node = rbtree_search_le(&symbol_tree, (void *)address);
	mutex_unlock(&symbol_tree_lock);

	if (!node)
		return NULL;

	return (struct symbol_node *)node;
}

// Insert module symbols into the global symbol tree
static int insert_module_symbols(struct mod *mod) {
	elf_md *e = mod->md;
	const Elf_Sym *symtab = e->mut_symbol_table
		? e->mut_symbol_table
		: e->symbol_table;

	if (!symtab)
		return -1;

	mutex_lock(&symbol_tree_lock);

	for (size_t i = 0; i < e->symbol_count; i++) {
		const Elf_Sym *sym = &symtab[i];

		// Skip symbols without names or valid addresses
		if (sym->st_name == 0 || sym->st_value == 0)
			continue;

		// Allocate node from slab
		struct symbol_node *node = slab_alloc(&symbol_cache);
		if (!node) {
			mutex_unlock(&symbol_tree_lock);
			return -1;
		}

		// Set node fields - pointers reference existing module data
		node->symbol_name = elf_symbol_name(e, sym);
		node->address = sym->st_value;
		node->size = sym->st_size;
		node->mod = mod;

		// Insert into tree
		rbtree_insert(&symbol_tree, &node->rb);
	}

	mutex_unlock(&symbol_tree_lock);
	return 0;
}

// Remove module symbols from the global symbol tree
static void remove_module_symbols(struct mod *mod) {
	elf_md *e = mod->md;
	const Elf_Sym *symtab = e->mut_symbol_table
		? e->mut_symbol_table
		: e->symbol_table;

	if (!symtab)
		return;

	mutex_lock(&symbol_tree_lock);

	for (size_t i = 0; i < e->symbol_count; i++) {
		const Elf_Sym *sym = &symtab[i];

		if (sym->st_name == 0 || sym->st_value == 0)
			continue;

		// Remove from tree by address
		struct rbnode *node = rbtree_remove(&symbol_tree, (void *)sym->st_value);

		if (node) {
			// Free the symbol node back to slab
			slab_free(&symbol_cache, node);
		}
	}

	mutex_unlock(&symbol_tree_lock);
}

elf_md *elf_mod_load(struct vnode *);

sysret sys_loadmod(int fd) {
	int perm = USR_READ;
	struct file *ofd = get_file(fd);
	if (ofd == nullptr)
		return -EBADF;
	if (!read_mode(ofd))
		return -EPERM;
	struct vnode *vnode = ofd->vnode;
	if (vnode->type != FT_NORMAL)
		return -ENOEXEC;

	elf_md *e = elf_mod_load(vnode);
	if (!e)
		return -ENOEXEC;

	struct mod *mod = malloc(sizeof(struct mod));
	mod->md = e;
	mod->refcnt = 1;
	mod->load_base = (uintptr_t)e->mmap;
	list_init(&mod->deps);

	const Elf_Sym *modinfo_sym = elf_find_symbol(e, "modinfo");
	if (!modinfo_sym)
		return -100;
	struct modinfo *modinfo = elf_sym_addr(e, modinfo_sym);
	if (!modinfo)
		return -101;

	mod->modinfo = modinfo;
	mod->name = modinfo->name;
	list_append(&loaded_mods, &mod->node);

	// Insert module symbols into symbol tree
	if (insert_module_symbols(mod) < 0) {
		// Insertion failed, but continue - module still functional
		// Symbols just won't be available for backtraces
	}

	modinfo->init(mod);

	return 0;
}

int unload_mod(struct mod *mod) {
	if (!mod)
		return -1;

	// Remove symbols from tree first
	remove_module_symbols(mod);

	// Call module fini if exists
	if (mod->modinfo && mod->modinfo->fini)
		mod->modinfo->fini(mod);

	// Remove from loaded modules list
	list_remove(&mod->node);

	// TODO: Free module memory, unmap pages, free ELF metadata
	// For now, just free the mod structure
	free(mod);

	return 0;
}

void proc_mods(struct file *ofd, void *) {
	proc_sprintf(ofd, "name start end\n");
	list_for_each_safe (&loaded_mods) {
		struct mod *mod = container_of(struct mod, node, it);
		elf_md *e = mod->md;
		uintptr_t mod_start = (uintptr_t)e->mmap;
		uintptr_t mod_end = (uintptr_t)PTR_ADD(e->mmap, e->mmap_size);
		proc_sprintf(ofd, "%s %zx %zx\n", mod->name, mod_start, mod_end);
	}
}

// Print all kernel and module symbols from the rbtree
// Format: <address> <size> <type> <name> [module]
void proc_kallsyms(struct file *ofd, void *) {
	mutex_lock(&symbol_tree_lock);

	// Start with the minimum node and iterate through the tree
	struct rbnode *node = rbtree_min(&symbol_tree);

	while (node) {
		struct symbol_node *sym = (struct symbol_node *)node;
		const char *mod_name = sym->mod ? sym->mod->name : "kernel";

		// Print: address size name [module]
		proc_sprintf(ofd, "%016zx %8zx %s [%s]\n",
			sym->address, sym->size, sym->symbol_name, mod_name);

		node = rbtree_successor(node);
	}

	mutex_unlock(&symbol_tree_lock);
}
