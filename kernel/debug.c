#include <assert.h>
#include <elf.h>
#include <ng/arch.h>
#include <ng/debug.h>
#include <ng/mod.h>
#include <ng/panic.h>
#include <ng/serial.h>
#include <ng/string.h>
#include <ng/syscalls.h>
#include <ng/vmm.h>
#include <nightingale.h>
#include <stdio.h>

void backtrace_all(void) {
	list_for_each (struct thread, th, &all_threads, all_threads) {
		if (th == running_thread)
			continue;
		printf("--- [%i:%i] (%s):\n", th->tid, th->proc->pid, th->proc->comm);
		backtrace_context(th->kernel_ctx);
		printf("\n");
	}
}

// static void print_perf_frame(uintptr_t bp, uintptr_t ip) {
//     struct mod_sym sym = elf_find_symbol_by_address(ip);
//     if (!sym.sym)
//         return;
//     const elf_md *md = sym.mod ? sym.mod->md : &elf_ngk_md;
//     const char *name = elf_symbol_name(md, sym.sym);
//
//     if (sym.mod) {
//         s2printf("%s`%s\n", sym.mod->name, name);
//     } else {
//         s2printf("%s\n", name);
//     }
// }
//
void print_perf_trace(uintptr_t bp, uintptr_t ip) {
	//     if (bp < 0xFFFF000000000000)
	//         return;
	//     backtrace(bp, ip, print_perf_frame);
	//     s2printf("1\n\n");
}

// hexdump memory

static char dump_byte_char(char c) { return isprint(c) ? c : '.'; }

static void print_byte_char_line(char *c) {
	for (int i = 0; i < 16; i++) {
		printf("%c", dump_byte_char(c[i]));
	}
}

int hexdump(size_t len, char ptr[len]) {
	char *p = ptr;
	char *line = ptr;

	for (size_t i = 0; i < len; i++) {
		if (i % 16 == 0)
			printf("%08lx: ", (uintptr_t)(p + i));
		if (vmm_virt_to_phy((uintptr_t)(p + i)) == ~0u) {
			printf("EOM");
			return 0;
		}
		printf("%02hhx ", p[i]);
		if (i % 16 == 7)
			printf(" ");
		if (i % 16 == 15) {
			printf("   ");
			print_byte_char_line(line);
			line = p + i + 1;
			printf("\n");
		}
	}
	return 0;
}

// random things

void break_point() {
	// This is called in assert() to give a place to put a
	// gdb break point
	int a = 10;
	volatile int *x = &a;
	*x = 20;
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
