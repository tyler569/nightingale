#include "stdio.h"
#include "x86_64.h"
#include <setjmp.h>

void print_backtrace_raw(uintptr_t rbp, uintptr_t rip) {
	if (this_cpu->printing_backtrace) {
		printf("Faulted in backtrace\n");
		return;
	}

	this_cpu->printing_backtrace = true;

	printf("Backtrace:\n");

	int frames = 0;

	while (rbp && frames++ < 25) {
		printf("  frame ip: %lx\n", rip);

		rip = *(uintptr_t *)(rbp + 8);
		rbp = *(uintptr_t *)rbp;
	}

	this_cpu->printing_backtrace = false;
}

void print_backtrace(frame_t *f) { print_backtrace_raw(f->rbp, f->rip); }

void backtrace_frame(frame_t *f) { print_backtrace_raw(f->rbp, f->rip); }

void backtrace_context(jmp_buf b) {
	print_backtrace_raw(b->__regs.bp, b->__regs.ip);
}

void print_frame(frame_t *f) {
	printf("rax %16lx rbx %16lx rcx %16lx rdx %16lx\n", f->rax, f->rbx, f->rcx,
		f->rdx);
	printf("rsi %16lx rdi %16lx rbp %16lx rsp %16lx\n", f->rsi, f->rdi, f->rbp,
		f->rsp);
	printf(" r8 %16lx  r9 %16lx r10 %16lx r11 %16lx\n", f->r8, f->r9, f->r10,
		f->r11);
	printf("r12 %16lx r13 %16lx r14 %16lx r15 %16lx\n", f->r12, f->r13, f->r14,
		f->r15);
	printf(" cs %16lx  ss %16lx\n", f->cs, f->ss);
	printf("int %16lx err %16lx rip %16lx flg %16lx\n", f->int_no, f->err_code,
		f->rip, f->rflags);
}
