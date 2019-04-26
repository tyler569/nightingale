
#ifndef NIGHTINGALE_MOD_H
#define NIGHTINGALE_MOD_H

#include <ng/basic.h>
#include <ng/elf.h>

typedef int (init_module_t)();

int load_module(Elf *elf, size_t len);

/* implemented by modules */

int init_module();

#endif

