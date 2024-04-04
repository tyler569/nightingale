#pragma once
#ifndef NG_PANIC_H
#define NG_PANIC_H

#include <stdio.h>
#include <sys/cdefs.h>

BEGIN_DECLS

void disable_irqs();
_Noreturn void halt();

__NOINLINE void break_point(void);
void backtrace_all(void);

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

#endif // NG_PANIC_H
