#include "x86_64.h"
#include <assert.h>
#include <ng/debug.h>
#include <ng/irq.h>
#include <ng/panic.h>
#include <ng/signal.h>
#include <ng/syscall.h>
#include <ng/thread.h>
#include <ng/vmm.h>
#include <ng/x86/apic.h>
#include <ng/x86/cpu.h>
#include <ng/x86/interrupt.h>
#include <ng/x86/pic.h>
#include <ng/x86/pit.h>
#include <stdio.h>

#define USING_PIC 0

#if USING_PIC
#define send_eoi pic_send_eoi
#else
#define send_eoi lapic_eoi
#endif

// Stack dumps are not particularly helpful in the general case. This could be
// a runtime option though, it's a good candidate for that system when it
// happens.
#define DO_STACK_DUMP 0

static void print_error_dump(interrupt_frame *r);

bool do_perf_trace = false;

bool doing_exception_print = false;

void panic_trap_handler(interrupt_frame *r);
void halt_trap_handler(interrupt_frame *r);

__USED
void c_interrupt_entry(interrupt_frame *r) {
	bool from_usermode = false;
	assert(r->cs == USER_CS || r->cs == KERNEL_CS);
	assert(r->ss == USER_SS || r->ss == KERNEL_SS || r->ss == 0);

	if (r->cs != 8) {
		from_usermode = true;
		running_thread->rsp = r->rsp;
		running_thread->user_ctx = r;
		running_thread->user_ctx_valid = true;
	}

	if (r->int_no == 1 && running_thread->tracer) {
		trace_report_trap(1);
	} else if (r->int_no == 3 && running_thread && running_thread->tracer) {
		trace_report_trap(3);
	} else if (r->int_no == 14) {
		page_fault(r);
	} else if (r->int_no == 127) {
		asm volatile("movl $0, %esp");
	} else if (r->int_no == 128) {
		do_syscall(
			r->rdi, r->rsi, r->rdx, r->rcx, r->r8, r->r9, (int)r->rax, r);
	} else if (r->int_no == 130) {
		panic_trap_handler(r);
	} else if (r->int_no == 131) {
		halt_trap_handler(r);
	} else if (r->int_no < 32) {
		generic_exception(r);
	} else if (r->int_no == 32) {
		// if (do_perf_trace)
		// 	print_perf_trace(r->rbp, r->rip);
		// timer interrupt needs to EOI first
		send_eoi(r->int_no - 32);
		irq_handler(r);
	} else if (r->int_no < 32 + NIRQS) {
		irq_handler(r);
		send_eoi(r->int_no - 32);
	}

	if (from_usermode)
		running_thread->user_ctx_valid = false;

	assert(r->cs == USER_CS || r->cs == KERNEL_CS);
	assert(r->ss == USER_SS || r->ss == KERNEL_SS || r->ss == 0);
}

void panic_trap_handler(interrupt_frame *r) {
	disable_irqs();
	printf("\n");
	printf("panic: trap at %#lx\n", r->rip);
	print_error_dump(r);
	panic();
}

void halt_trap_handler(interrupt_frame *r) {
	disable_irqs();
	printf("\nhalt: trap cpu %i at %#lx\n", cpu_id(), r->rip);

	// Cannot use panic() or halt() since those will send more IPIs
	while (true)
		asm volatile("hlt");
}

static void print_error_dump(interrupt_frame *r) {
	static spin_lock_t lock = { 0 };

	spin_lock(&lock);
	uintptr_t ip = r->rip;
	uintptr_t bp = r->rbp;
	printf("(CPU %i) Fault occurred at %#lx\n", cpu_id(), ip);
	print_registers(r);
	// printf("backtrace from: %#lx\n", bp);
	backtrace_frame(r);

	if (r != running_thread->user_ctx && running_thread->user_ctx_valid) {
		// printf("user backtrace from: %#lx\n", running_thread->user_ctx->rbp);
		backtrace_frame(running_thread->user_ctx);
	}

#if DO_STACK_DUMP
	printf("Stack dump: (sp at %#lx)\n", real_sp);
	dump_mem((char *)real_sp - 64, 128);
#endif
	spin_unlock(&lock);
}

[[noreturn]] static void kill_for_unhandled_interrupt(interrupt_frame *r) {
	if ((r->cs & 3) > 0) {
		// died in usermode
		signal_self(SIGSEGV);
	} else if (running_process->pid > 0) {
		// died in kernel mode in a user process
		printf("Would signal SEGV, but we decided that was a bad idea\n");
		kill_process(running_process, 128 + SIGSEGV);
	} else {
		// died in kernel mode
		panic();
	}
	UNREACHABLE();
}

void page_fault(interrupt_frame *r) {
	uintptr_t fault_addr = read_cr2();
	uint64_t code = r->err_code;
	const char *reason, *rw, *mode, *type;

	if (vmm_do_page_fault(fault_addr, code) == FAULT_CONTINUE) {
		// handled and able to return
		return;
	}

	if (r->err_code & F_RESERVED) {
		printf("Fault was caused by writing to a reserved field\n");
	}
	reason = code & F_PRESENT ? "protection violation" : "page not present";
	rw = code & F_WRITE ? "writing" : "reading";
	mode = code & F_USERMODE ? "user" : "kernel";
	type = code & F_IFETCH ? "instruction" : "data";

	if (running_thread)
		printf("Thread: [%i:%i] (\"%s\") performed an access violation\n",
			running_process->pid, running_thread->tid, running_process->comm);
	else
		printf("Pre-thread environment performed an access violation\n");

	const char *sentence = "Fault %s %s:%#lx because %s from %s mode.\n";
	printf(sentence, rw, type, fault_addr, reason, mode);

	if (fault_addr < 0x1000)
		printf("nullptr pointer access?\n");
	break_point();
	print_error_dump(r);
	if (code & F_USERMODE)
		signal_self(SIGSEGV);
	kill_for_unhandled_interrupt(r);
}

void generic_exception(interrupt_frame *r) {
	printf("Thread: [%i:%i] (\"%s\") experienced a fault\n",
		running_process->pid, running_thread->tid, running_process->comm);
	printf("Unhandled exception at %#lx\n", r->rip);
	printf("Fault: %s (%s), error code: %#04lx\n", exception_codes[r->int_no],
		exception_reasons[r->int_no], r->err_code);

	break_point();
	print_error_dump(r);
	kill_for_unhandled_interrupt(r);
}

void unhandled_interrupt_handler(interrupt_frame *r) { }

/* Utility functions */

void enable_irqs() {
	// running_thread->irq_disable_depth -= 1;
	// assert(running_thread->irq_disable_depth >= 0);
	// if (running_thread->irq_disable_depth == 0)
	asm volatile("sti");
}

void disable_irqs() {
	asm volatile("cli");
	// running_thread->irq_disable_depth += 1;
}

bool irqs_are_disabled() {
	long flags;
	asm volatile("pushfq; pop %0" : "=r"(flags));
	if (flags & 0x200) {
		assert(running_thread->irq_disable_depth == 0);
		return false;
	} else {
		assert(running_thread->irq_disable_depth > 0);
		return true;
	}
}

const char *exception_codes[] = {
	"#DE",
	"#DB",
	"NMI",
	"#BP",
	"#OF",
	"#BR",
	"#UD",
	"#NM",
	"#DF",
	"<none>",
	"#TS",
	"#NP",
	"#SS",
	"#GP",
	"#PF",
	"<reserved>",
	"#MF",
	"#AC",
	"#MC",
	"#XM",
	"#VE",
	"<reserved>",
	"<reserved>",
	"<reserved>",
	"<reserved>",
	"<reserved>",
	"<reserved>",
	"<reserved>",
	"<reserved>",
	"<reserved>",
	"#SX",
	"<reserved>",
};

const char *exception_reasons[] = {
	"Divide by zero",
	"Debug",
	"Non-maskable Interrupt",
	"Breakpoint",
	"Overflow Trap",
	"Bound Range Exceeded",
	"Invalid Opcode",
	"Device Not Available",
	"Double Fault",
	"Coprocessor Segment Overrun (Deprecated)",
	"Invalid TSS",
	"Segment Not Present",
	"Stack-Segment Fault",
	"General Protection Fault",
	"Page Fault",
	"Reserved",
	"x87 Floating Point Exception",
	"Alignment Check",
	"Machine Check",
	"SIMD Floating-Point Exception",
	"Virtualization Exception",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Security Exception",
	"Reserved",
};
