
#ifndef NIGHTINGALE_ARCH_MEMMAP_H
#define NIGHTINGALE_ARCH_MEMMAP_H

#include <basic.h>

#if 0
// TODO
#if X86_64
#include <arch/x86/64/memmap.h>
#elif I686
#include <arch/x86/32/memmap.h>
#endif
#endif

#if X86_64

# define KERNEL_STACKS_START 0xFFFFFFFF85000000
# define NET_BUFFER          0xFFFFFFFF84000000
# define KERNEL_HEAP_START   0xFFFFFF0000000000
# define USER_STACK              0x7FFFFF000000
# define USER_ARGV               0x7FFFFF001000
# define USER_ENVP               0x7FFFFF002000
# define USER_MMAP_BASE          0x180000000000

#elif I686

# define KERNEL_STACKS_START         0x85000000
# define NET_BUFFER                  0x84000000
# define KERNEL_HEAP_START           0xC0000000
# define USER_STACK                  0x7FFF0000
# define USER_ARGV                   0x7FFF1000
# define USER_ENVP                   0x7FFF2000
# define USER_MMAP_BASE              0x50000000

#endif

#endif // include guard

