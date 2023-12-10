#include <elf.h>
#include <list.h>
#include <ng/common.h>
#include <ng/fs.h>
#include <ng/mod.h>
#include <ng/syscalls.h>
#include <stdlib.h>

struct list loaded_mods = LIST_INIT(loaded_mods);

struct mod_sym elf_find_symbol_by_address(uintptr_t address)
{
    struct mod *in_mod = nullptr;
    elf_md *in_elf = &elf_ngk_md;
    list_for_each (mod, m, &loaded_mods, node) {
        elf_md *e = m->md;
        auto mod_start = (uintptr_t)e->mmap;
        auto mod_end = (uintptr_t)PTR_ADD(e->mmap, e->mmap_size);
        if (address >= mod_start && address < mod_end) {
            in_mod = m;
            in_elf = e;
            break;
        }
    }
    const Elf_Sym *s = elf_symbol_by_address(in_elf, address);
    if (s) {
        return (mod_sym) { in_mod, s };
    } else {
        return (mod_sym) {};
    }
}

extern "C" elf_md *elf_mod_load(struct inode *);

sysret sys_loadmod(int fd)
{
    int perm = USR_READ;
    struct file *ofd = get_file(fd);
    if (ofd == nullptr)
        return -EBADF;
    if (!read_mode(ofd))
        return -EPERM;
    struct inode *inode = ofd->inode;
    if (inode->type != FT_NORMAL)
        return -ENOEXEC;

    elf_md *e = elf_mod_load(inode);
    if (!e)
        return -ENOEXEC;

    mod *m = static_cast<mod *>(malloc(sizeof(struct mod)));
    m->md = e;
    m->refcnt = 1;
    m->load_base = (uintptr_t)e->mmap;
    list_init(&m->deps);

    const Elf_Sym *modinfo_sym = elf_find_symbol(e, "minfo");
    if (!modinfo_sym)
        return -100;
    auto *minfo = static_cast<modinfo *>(elf_sym_addr(e, modinfo_sym));
    if (!minfo)
        return -101;

    m->modinfo = minfo;
    m->name = minfo->name;
    list_append(&loaded_mods, &m->node);
    minfo->init(m);

    return 0;
}

extern "C" void proc_mods(struct file *ofd, void *)
{
    proc_sprintf(ofd, "name start end\n");
    list_for_each (mod, m, &loaded_mods, node) {
        elf_md *e = m->md;
        auto mod_start = (uintptr_t)e->mmap;
        auto mod_end = (uintptr_t)PTR_ADD(e->mmap, e->mmap_size);
        proc_sprintf(ofd, "%s %zx %zx\n", m->name, mod_start, mod_end);
    }
}
