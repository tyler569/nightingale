#pragma once

#include <ng/arch-2.h>
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
		printf("[PANIC] " __VA_ARGS__); \
		halt_forever(); \
		__builtin_unreachable(); \
	} while (0)

#define panic_bt panic

END_DECLS
