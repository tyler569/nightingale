#include <nightingale.h>
#include <stdio.h>
#include <sys/frame.h>

#if __kernel__
#include <ng/debug.h>
#include <ng/thread.h>
#endif

void print_registers(interrupt_frame *r) {
	printf("ax %016lx bx %016lx cx %016lx dx %016lx\n", r->rax, r->rbx, r->rcx,
		r->rdx);
	printf("sp %016lx bp %016lx si %016lx di %016lx\n", r->rsp, r->rbp, r->rsi,
		r->rdi);
	printf(" 8 %016lx  9 %016lx 10 %016lx 11 %016lx\n", r->r8, r->r9, r->r10,
		r->r11);
	printf("12 %016lx 13 %016lx 14 %016lx 15 %016lx\n", r->r12, r->r13, r->r14,
		r->r15);
	printf("ip %016lx fl %016lx [", r->rip, r->rflags);
	printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c]\n",
		r->rflags & 0x00000001 ? 'C' : ' ', r->rflags & 0x00000004 ? 'P' : ' ',
		r->rflags & 0x00000010 ? 'A' : ' ', r->rflags & 0x00000040 ? 'Z' : ' ',
		r->rflags & 0x00000080 ? 'S' : ' ', r->rflags & 0x00000100 ? 'T' : ' ',
		r->rflags & 0x00000200 ? 'I' : ' ', r->rflags & 0x00000400 ? 'D' : ' ',
		r->rflags & 0x00000800 ? 'O' : ' ', r->rflags & 0x00010000 ? 'R' : ' ',
		r->rflags & 0x00020000 ? 'V' : ' ', r->rflags & 0x00040000 ? 'a' : ' ',
		r->rflags & 0x00080000 ? 'v' : ' ', r->rflags & 0x00100000 ? 'v' : ' ');
#ifdef __kernel__
	printf("cr3 %15lx pid %i:%i\n", cr3(), running_process->pid,
		running_thread->tid);
#endif
}
