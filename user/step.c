#include <elf.h>
#include <errno.h>
#include <fcntl.h>
#include <nightingale.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/trace.h>
#include <sys/wait.h>
#include <unistd.h>

#include "ng/x86/cpu.h"

int exec(char **args) {
	int child = fork();
	if (child)
		return child;

	// strace(1);
	trace(TR_TRACEME, 0, nullptr, nullptr);
	raise(SIGSTOP);

	return execve(args[0], args, nullptr);
}

noreturn void fail(const char *str) {
	perror(str);
	exit(1);
}

int main(int argc, char **argv) {
	if (!argv[1]) {
		fprintf(stderr, "No command specified\n");
		exit(EXIT_FAILURE);
	}

	char **child_args = argv + 1;

	struct stat child_exec_stat;
	int child_exec = open(*child_args, O_RDONLY);
	if (child_exec < 0) {
		perror("open");
		exit(1);
	}
	int err = fstat(child_exec, &child_exec_stat);
	if (err < 0) {
		perror("fstat");
		exit(1);
	}
	off_t child_buf_len = child_exec_stat.st_size;

	void *child_buf
		= mmap(nullptr, child_buf_len, PROT_READ, MAP_PRIVATE, child_exec, 0);
	elf_md *child_elf = elf_parse(child_buf, child_buf_len);

	interrupt_frame r;
	int child = exec(child_args);
	int status;

	wait(&status);
	trace(TR_SINGLESTEP, child, nullptr, nullptr);

	while (true) {
		wait(&status);
		if (errno)
			fail("wait");
		if (status < 256)
			exit(0);

		int event = status & ~0xFFFF;
		int syscall = status & 0xFFFF;
		intptr_t signal = 0;

		trace(TR_GETREGS, child, nullptr, &r);

		if (event == TRACE_SYSCALL_ENTRY) {
			printf("syscall_enter: %s\n", syscall_names[syscall]);
		}

		if (event == TRACE_SYSCALL_EXIT) {
			printf("syscall_exit: %s", syscall_names[syscall]);
			printf(" -> %zu\n", FRAME_RETURN(&r));
		}

		if (event == TRACE_SIGNAL) {
			printf("signal: %i\n", syscall);
			signal = syscall;
		}

		if (event == TRACE_TRAP) {
			const Elf_Sym *sym = elf_symbol_by_address(child_elf, r.ip);
			const char *sym_name = elf_symbol_name(child_elf, sym);
			printf("step: %#10zx (%s)\n", r.ip, sym_name);
		}

		trace(TR_SINGLESTEP, child, nullptr, (void *)signal);
	}
}
