#include <elf.h>
#include <list.h>
#include <ng/fs.h>
#include <ng/mod.h>
#include <stdlib.h>

struct list_head loaded_mods = LIST_INIT(loaded_mods);

struct mod_sym elf_find_symbol_by_address(uintptr_t address) {
	struct mod *in_mod = nullptr;
	elf_md *in_elf = &elf_ngk_md;
	list_for_each_safe (&loaded_mods) {
		struct mod *mod = container_of(struct mod, node, it);
		elf_md *e = mod->md;
		uintptr_t mod_start = (uintptr_t)e->mmap;
		uintptr_t mod_end = (uintptr_t)PTR_ADD(e->mmap, e->mmap_size);
		if (address >= mod_start && address < mod_end) {
			in_mod = mod;
			in_elf = e;
			break;
		}
	}
	const Elf_Sym *s = elf_symbol_by_address(in_elf, address);
	if (s) {
		return (struct mod_sym) { in_mod, s };
	} else {
		return (struct mod_sym) { 0, 0 };
	}
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
	modinfo->init(mod);

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
