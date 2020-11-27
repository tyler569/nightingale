#include <basic.h>
#include <elf.h>
#include <errno.h>
#include <list.h>
#include <ng/dmgr.h>
#include <ng/fs.h>
#include <ng/mod.h>
#include <ng/syscall.h>
#include <ng/thread.h>
#include <ng/vmm.h>
#include <stdio.h>
#include <stdlib.h>

extern elf_md elf_ngk_md;

struct list loaded_mods = {0};

struct mod_sym {
    elf_md *mod;
    Elf_Sym *sym;
};

struct mod_sym elf_find_symbol_by_address(uintptr_t address) {
    // TODO: check symbol range and find a module for it

    Elf_Shdr *symtab_header = elf_ngk_md.symbol_table_section;
    size_t nsymbols = symtab_header->sh_size / symtab_header->sh_entsize;
    Elf_Sym *symtab = elf_ngk_md.symbol_table;

    Elf_Sym *best_match;
    bool found = false;
    long best_offset = LONG_MIN;

    for (size_t i = 0; i < nsymbols; i++) {
        Elf_Sym *sym = symtab + i;

        if (sym->st_name == 0) continue;
        if (sym->st_value > address) continue;
        long offset = address - sym->st_value;

        if (offset < best_offset) {
            best_offset = offset;
            best_match = sym;
            found = true;
        }
    }

    if (found) {
        return (struct mod_sym){&elf_ngk_md, best_match};
    } else {
        return (struct mod_sym){0, 0};
    }
}

const char *elf_mod_symbol_name(struct mod_sym sym) {
    return elf_symbol_name(sym.mod, sym.sym);
}

elf_md *elf_mod_load(struct file *);

sysret sys_loadmod(int fd) {
    int perm = USR_READ;
    struct open_file *ofd = dmgr_get(&running_process->fds, fd);
    if (ofd == NULL) { return -EBADF; }
    if ((ofd->flags & perm) != perm) { return -EPERM; }
    struct file *file = ofd->node;
    if (file->filetype != FT_BUFFER) { return -ENOEXEC; }

    elf_md *e = elf_mod_load(file);

    // TODO: create a `struct mod` and store it

    Elf_Sym *modinfo_sym = elf_find_symbol(e, "modinfo");
    if (!modinfo_sym) return -100;
    struct modinfo *modinfo = elf_sym_addr(e, modinfo_sym);
    if (!modinfo) return -101;

    printf("modinfo: %p\ninit: %p\n", modinfo, modinfo->init);
    modinfo->init(NULL);

    return 0;
}
