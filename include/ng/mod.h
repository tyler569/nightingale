
#ifndef NG_MOD_H
#define NG_MOD_H

#include <ng/basic.h>
#include <ng/elf.h>
#include <ds/list.h>
#include <stdatomic.h>

typedef int (init_mod_t)(int);

struct mod {
        const char *name;

        Elf *blob;
        Elf_Shdr *symtab;
        Elf_Shdr *strtab;

        struct list deps;
        atomic_int refcnt;

        init_mod_t *init_fn;
};

extern Elf_Shdr *ngk_symtab;
extern Elf_Shdr *ngk_strtab;

extern struct list loaded_mods;

int load_mod(Elf *elf, size_t len);
int unload_mod(struct mod *mod); // not implemented


/* implemented by modules */

int init_mod(int);

#endif

