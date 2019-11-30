
#pragma once
#ifndef NG_MEMMAP_H
#define NG_MEMMAP_H

#include <basic.h>

#if 0
// TODO
#if X86_64
#include <ng/x86/64/memmap.h>
#elif I686
#include <ng/x86/32/memmap.h>
#endif
#endif

#if X86_64

#define KERNEL_RESERVABLE_SPACE 0xFFFFFFFFC0000000
#define SIGRETURN_THUNK             0x7FFFFF021000
#define USER_ENVP                   0x7FFFFF011000 // to 21000, 16x4K
#define USER_ARGV                   0x7FFFFF001000 // to 11000, 16x4K
#define USER_STACK                  0x7FFFFF000000 // 0000 - 0FFF is guard
#define USER_MMAP_BASE              0x100000000000

#elif I686

#define KERNEL_RESERVABLE_SPACE         0xC0000000
#define SIGRETURN_THUNK                 0x7FF21000
#define USER_ENVP                       0x7FF11000 // to 21000, 16x4K
#define USER_ARGV                       0x7FF01000 // to 11000, 16x4K
#define USER_STACK                      0x7FF00000 // 0000 - 0FFF is guard
#define USER_MMAP_BASE                  0x10000000

#endif

#endif // NG_MEMMAP_H

