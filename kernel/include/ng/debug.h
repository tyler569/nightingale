#pragma once

#include <assert.h>
#include <hexdump.h>
#include <ng/arch.h>
#include <setjmp.h>
#include <stdio.h>
#include <sys/cdefs.h>

#ifdef DEBUG

#define do_debug true
#define DEBUG_PRINTF(...) \
	do { \
		printf("[DEBUG] " __VA_ARGS__); \
	} while (0)

#else // !DEBUG

#define do_debug false
#define DEBUG_PRINTF(...)

#endif // DEBUG

#define WARN_PRINTF(...) \
	do { \
		printf("[WARN!] " __VA_ARGS__); \
	} while (0)

#define UNREACHABLE() assert("not reachable" && 0)

#ifdef __x86_64__
#define HIGHER_HALF 0x800000000000
#else
#define HIGHER_HALF 0x80000000
#endif

BEGIN_DECLS

struct interrupt_frame;

void backtrace(uintptr_t bp, uintptr_t ip);
void backtrace_from_with_ip(uintptr_t bp, uintptr_t ip);
void backtrace_frame(struct interrupt_frame *frame);
void backtrace_context(jmp_buf ctx);

__NOINLINE void break_point();

END_DECLS
