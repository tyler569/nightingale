#pragma once
#ifndef NG_MOD_H
#define NG_MOD_H

#include <elf.h>
#include <list.h>
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

struct mod_sym elf_find_symbol_by_address(uintptr_t address);

END_DECLS

#endif // NG_MOD_H
