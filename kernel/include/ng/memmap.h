#pragma once

#include <sys/cdefs.h>

#define KERNEL_RESERVABLE_SPACE 0xFFFFFFFFC0000000
// USER_ARGV and USER_ENVP removed - args/envp now on stack (System V ABI)
#define USER_STACK 0x7FFFFF000000 // 0000 - 0FFF is guard
#define USER_MMAP_BASE 0x100000000000
