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
