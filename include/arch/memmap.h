
#ifndef NG_ARCH_MEMMAP_H
#define NG_ARCH_MEMMAP_H

#include <ng/basic.h>

#if 0
// TODO
#if X86_64
#include <arch/x86/64/memmap.h>
#elif I686
#include <arch/x86/32/memmap.h>
#endif
#endif

#if X86_64

// added dynamic mechanism - these can go soon

// #define KERNEL_STACKS_START     0xFFFFFFFF85000000
// #define NET_BUFFER              0xFFFFFFFF84000000
// #define KERNEL_HEAP_BUFFER      0xFFFFFFFE00000000
//                 // current max: 0xFFFFFFFE01000000
// #define KERNEL_LIST_BUFFER      0xFFFFFFFD00000000
// #define KERNEL_PAGE_TABLES      0xFFFF000000000000 // - 0xFFFFFF8000000000
#define KERNEL_RESERVABLE_SPACE 0xFFFFFFF000000000
#define USER_STACK                  0x7FFFFF000000 // 0000 - 0FFF is guard
#define USER_ARGV                   0x7FFFFF001000
#define USER_ENVP                   0x7FFFFF002000
#define USER_MMAP_BASE              0x700000000000

#elif I686

// #define KERNEL_STACKS_START             0x85000000
// #define NET_BUFFER                      0x84000000
// #define KERNEL_HEAP_BUFFER              0xC0000000
//                         // current max: 0xC1000000
// #define KERNEL_LIST_BUFFER              0xE0000000
// #define KERNEL_PAGE_TABLES              0xFF800000 // - 0xFFFFFFFF
#define KERNEL_RESERVABLE_SPACE         0xC0000000
#define USER_STACK                      0x7FFF0000 // 0000 - 0FFF is guard
#define USER_ARGV                       0x7FFF1000
#define USER_ENVP                       0x7FFF2000
#define USER_MMAP_BASE                  0x50000000

#endif

#endif // include guard
