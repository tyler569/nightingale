#pragma once
#ifndef NG_MOD_H
#define NG_MOD_H

#include <basic.h>
#include <linker/elf.h>
#include <list.h>

enum modinit_status {
    MODINIT_SUCCESS,
    MODINIT_FAILURE,
};

struct mod;

struct modinfo {
    const char *name;
    enum modinit_status (*init)(struct mod *);
    void (*fini)(struct mod *);
};

struct mod {
    const char *name;

    Elf *blob;
    Elf_Shdr *symtab;
    Elf_Shdr *strtab;

    struct list deps;
    int refcnt;

    struct modinfo *modinfo;
};

extern Elf_Shdr *ngk_symtab;
extern Elf_Shdr *ngk_strtab;

extern struct list loaded_mods;

int load_mod(Elf *elf, size_t len);
int unload_mod(struct mod *mod); // not implemented

#endif // NG_MOD_H
