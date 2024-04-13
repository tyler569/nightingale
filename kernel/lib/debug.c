#include <assert.h>
#include <ng/arch.h>
#include <ng/common.h>
#include <ng/debug.h>
#include <ng/panic.h>
#include <ng/syscalls.h>
#include <stdio.h>

void backtrace_all() {
	list_for_each_safe (&all_threads) {
		struct thread *th = container_of(struct thread, all_threads, it);
		if (th == running_thread)
			continue;
		printf("--- [%i:%i] (%s):\n", th->tid, th->proc->pid, th->proc->comm);
		backtrace_context(th->kernel_ctx);
		printf("\n");
	}
}

static char dump_byte_char(char c) { return isprint(c) ? c : '.'; }

static void print_byte_char_line(const char *c, size_t remaining_len) {
	for (int i = 0; i < remaining_len; i++) {
		printf("%c", dump_byte_char(c[i]));
	}
}

static void hexdump_line(
	const void *data, size_t remaining_len, uintptr_t base_address) {
	printf("%#08lx: ", base_address);
	size_t i;
	for (i = 0; i < remaining_len; i++) {
		printf("%02hhx ", ((const uint8_t *)data)[i]);
		if (i == 7)
			printf(" ");
	}
	for (; i < 16; i++) {
		printf("   ");
		if (i == 7)
			printf(" ");
	}
	printf("  ");
	print_byte_char_line(data, remaining_len);
	printf("\n");
}

void hexdump(const void *data, size_t len, uintptr_t base_address) {
	for (size_t i = 0; i < len; i += 16) {
		hexdump_line(data + i, MIN(len - i, 16), base_address + i);
	}
}

__NOINLINE void break_point() {
	// This is called in assert() to give a place to put a
	// gdb break point
}

sysret sys_fault(enum fault_type type) {
	volatile int *x = 0;
	switch (type) {
	case NULL_DEREF:
		return *x;
	case ASSERT:
		assert(0);
		break;
	default:
		return -EINVAL;
	}
}
