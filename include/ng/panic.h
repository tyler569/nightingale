#pragma once

#include <stdio.h>
#include <sys/cdefs.h>

BEGIN_DECLS

void disable_irqs();
[[noreturn]] void halt();

__NOINLINE void break_point();
void backtrace_all();

#define panic(...) \
	do { \
		break_point(); \
		disable_irqs(); \
		printf("[PANIC] " __VA_ARGS__); \
		halt(); \
		__builtin_unreachable(); \
	} while (0)

#define panic_bt(...) \
	do { \
		break_point(); \
		disable_irqs(); \
		printf("[PANIC] " __VA_ARGS__); \
		__asm__ volatile("int $0x82"); \
		halt(); \
	} while (0)

END_DECLS
