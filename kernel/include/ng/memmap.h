#pragma once
#ifndef NG_MEMMAP_H
#define NG_MEMMAP_H

#include <basic.h>

#define KERNEL_RESERVABLE_SPACE 0xFFFFFFFFC0000000
#define SIGRETURN_THUNK             0x7FFFFF021000
#define USER_ENVP                   0x7FFFFF011000 // to 21000, 16x4K
#define USER_ARGV                   0x7FFFFF001000 // to 11000, 16x4K
#define USER_STACK                  0x7FFFFF000000 // 0000 - 0FFF is guard
#define USER_MMAP_BASE              0x100000000000

#endif // NG_MEMMAP_H
