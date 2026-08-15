#pragma once

#include <ng/arch.h>
#include <stdio.h>
#include <sys/cdefs.h>

BEGIN_DECLS

[[noreturn]] void halt();

__NOINLINE void break_point();
void backtrace_all();

#define panic(...) \
	do { \
		break_point(); \
		arch_disable_irqs(); \
		printf("[PANIC] " __VA_ARGS__); \
		halt(); \
		__builtin_unreachable(); \
	} while (0)

#define panic_bt(...) \
	do { \
		break_point(); \
		arch_disable_irqs(); \
		printf("[PANIC] " __VA_ARGS__); \
		asm volatile("int $0x82"); \
		halt(); \
	} while (0)

END_DECLS
