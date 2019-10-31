
#pragma once
#ifndef NIGHTINGALE_PRINT_H
#define NIGHTINGALE_PRINT_H

#include <ng/basic.h>
#include <nc/stdio.h>

void debug_print_mem(size_t, void *);
void debug_dump(void *);
void debug_dump_after(void *);

#endif

