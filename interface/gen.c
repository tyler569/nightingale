#include <stdio.h>
#include <string.h>

struct arg {
	const char *name;
	const char *type;
};

struct syscall {
	int id;
	const char *return_type;
	const char *name;
	int nargs;
	struct arg args[6];
	bool frame;
};

struct errno {
	int id;
	const char *name;
	const char *perror_string;
};

#define SYSCALL(id, return_type, name, ...) \
	{ \
		id, #return_type, #name, \
			sizeof((struct arg[]) { __VA_ARGS__ }) / sizeof(struct arg), { \
			__VA_ARGS__ \
		}, false \
	}

#define SYSCALL_FRAME(id, return_type, name, ...) \
	{ \
		id, #return_type, #name, \
			sizeof((struct arg[]) { __VA_ARGS__ }) / sizeof(struct arg), { \
			__VA_ARGS__ \
		}, true \
	}

#define A(name, type) \
	{ #type, #name }

static struct syscall syscalls[] = {
	SYSCALL(2, noreturn void, _exit, A(int, exit_code)),
	SYSCALL(6, pid_t, fork),
	SYSCALL(7, int, top, A(int, show_threads)),
	SYSCALL(8, pid_t, getpid),
	SYSCALL(9, pid_t, gettid),
	SYSCALL_FRAME(10, int, execve, A(const char *, program), A(char **, argv),
		A(char **, envp)),
	SYSCALL(15, int, syscall_trace, A(pid_t, pid), A(int, state)),
	SYSCALL(22, int, waitpid, A(pid_t, pid), A(int *, exit_code),
		A(enum wait_options, options)),
	SYSCALL(24, int, uname, A(struct utsname *, uname)),
	SYSCALL(25, int, yield),
	SYSCALL(28, void *, mmap, A(void *, addr), A(size_t, len), A(int, prot),
		A(int, flags), A(int, fd), A(off_t, offset)),
	SYSCALL(29, int, munmap, A(void *, addr), A(size_t, len)),
	SYSCALL(31, int, setpgid, A(pid_t, pid), A(pid_t, pgid)),
	SYSCALL(32, void, exit_group, A(int, exit_code)),
	SYSCALL_FRAME(33, pid_t, clone0, A(clone_fn, fn), A(void *, new_stack),
		A(int, flags), A(void *, arg)),
	SYSCALL(34, int, loadmod, A(int, fd)),
	SYSCALL(35, int, haltvm, A(int, exit_code)),
	SYSCALL_FRAME(37, int, execveat, A(int, fd), A(const char *, program),
		A(char **, argv), A(char **, envp)),
	SYSCALL(41, sighandler_t, sigaction, A(int, sig), A(sighandler_t, handler),
		A(int, flags)),
	SYSCALL(42, int, sigreturn, A(int, code)),
	SYSCALL(43, int, kill, A(pid_t, pid), A(int, dig)),
	SYSCALL(44, int, sleepms, A(int, ms)),
	SYSCALL(46, long, xtime),
	SYSCALL(47, pid_t, create, A(const char *, executable)),
	SYSCALL(48, int, procstate, A(pid_t, pid), A(enum procstate, flags)),
	SYSCALL(49, int, fault, A(enum fault_type, fault)),
	SYSCALL(50, int, trace, A(enum trace_command, cmd), A(pid_t, pid),
		A(void *, addr), A(void *, data)),
	SYSCALL(51, int, sigprocmask, A(int, op), A(const sigset_t *, new),
		A(sigset_t *, old)),
	SYSCALL(58, noreturn void, exit_thread, A(int, exit_code)),
	SYSCALL(60, int, btime, A(time_t *, time), A(struct tm *, tm)),
	SYSCALL(61, int, openat, A(int, fd), A(const char *, path), A(int, flags),
		A(int, mode)),
	SYSCALL(62, int, mkdirat, A(int, fd), A(const char *, path), A(int, mode)),
	SYSCALL(63, int, close, A(int, fd)),
	SYSCALL(
		64, int, pathname, A(int, fd), A(const char *, buf), A(size_t, len)),
	SYSCALL(65, ssize_t, getdents, A(int, fd), A(struct dirent *, buf),
		A(size_t, len)),
	SYSCALL(66, ssize_t, read, A(int, fd), A(void *, buffer), A(size_t, len)),
	SYSCALL(67, ssize_t, write, A(int, fd), A(const void *, buffer),
		A(size_t, len)),
	SYSCALL(68, int, fstat, A(int, fd), A(struct stat *, statbuf)),
	SYSCALL(69, int, linkat, A(int, oldfdat), A(const char *, oldpath),
		A(int, newfdat), A(const char *, newpath)),
	SYSCALL(70, int, symlinkat, A(const char *, topath), A(int, newfdat),
		A(const char *, newpath)),
	SYSCALL(71, int, readlinkat, A(int, fdat), A(const char *, path),
		A(char *, buffer), A(size_t, len)),
	SYSCALL(72, int, mknodat, A(int, fdat), A(const char *, path),
		A(mode_t, mode), A(dev_t, device)),
	SYSCALL(73, int, pipe, A(int *, pipefds)),
	SYSCALL(74, int, ioctl, A(int, fd), A(int, request), A(void *, argp)),
	SYSCALL(75, off_t, lseek, A(int, fd), A(off_t, offset), A(int, whence)),
	SYSCALL(76, int, mountat, A(int, atfd), A(const char *, target),
		A(int, type), A(int, s_atfd), A(const char *, source)),
	SYSCALL(77, int, dup, A(int, fd)),
	SYSCALL(78, int, dup2, A(int, fd), A(int, newfd)),
	SYSCALL(79, int, fchmod, A(int, fd), A(int, mode)),
	SYSCALL(
		80, int, chmodat, A(int, atfd), A(const char *, path), A(int, mode)),
	SYSCALL(81, int, unlinkat, A(int, atfd), A(const char *, path)),
	SYSCALL(82, int, statat, A(int, atfd), A(const char *, path),
		A(struct stat *, statbuf)),
	SYSCALL(83, int, mkpipeat, A(int, atfd), A(const char *, path),
		A(mode_t, mode)),
	SYSCALL(84, ssize_t, getcwd, A(char *, buffer), A(size_t, len)),
	SYSCALL(85, int, settls, A(void *, base)),
	SYSCALL(86, int, chdirat, A(int, atfd), A(const char *, path)),
	SYSCALL(87, int, report_events, A(long, event_mask)),
	SYSCALL(88, int, submit, A(struct submission *, queue), A(size_t, len)),
};

// pointers are handled separately; anything with a '*' is considered a pointer
const char *argument_formats[][2] = {
	{ "int", "%i" },
	{ "long", "%li" },
	{ "unsigned int", "%u" },
	{ "unsigned long", "%lu" },
	{ "size_t", "%zu" },
	{ "ssize_t", "%zi" },
	{ "mode_t", "%o" },
	{ "dev_t", "%u" },
	{ "time_t", "%li" },
	{ "off_t", "%li" },
	{ "pid_t", "%i" },
	{ "sighandler_t", "%p" },
	{ "clone_fn", "%p" },
};

#define ERRNO(id, name, perror_string) \
	{ id, #name, perror_string }

static struct errno errnos[] = {};

enum {
	NUM_SYSCALLS = sizeof(syscalls) / sizeof(struct syscall),
	NUM_ERRNOS = sizeof(errnos) / sizeof(struct errno),

	SYSCALL_TABLE_SIZE = 256,
};

int arg_is_pointer(const struct arg *arg) {
	return strchr(arg->type, '*') != NULL;
}

const char *get_format(const struct arg *arg) {
	if (arg_is_pointer(arg)) {
		return "%p";
	}
	if (strstr(arg->type, "sighandler_t") != NULL) {
		return "%p";
	}
	if (strstr(arg->type, "clone_fn") != NULL) {
		return "%p";
	}
	if (strstr(arg->type, "enum") != NULL) {
		return "%i";
	}

	for (int i = 0; i < sizeof(argument_formats) / sizeof(argument_formats[0]);
		 i++) {
		if (strcmp(argument_formats[i][0], arg->type) == 0) {
			return argument_formats[i][1];
		}
	}

	printf("warning: unknown format for type %s\n", arg->type);
	return "%i";
}

void print_enum(FILE *file) {
	fprintf(file, "enum syscalls {\n");
	for (int i = 0; i < NUM_SYSCALLS; i++) {
		fprintf(file, "\tNG_%s = %d,\n", syscalls[i].name, syscalls[i].id);
	}
	fprintf(file, "};\n");
}

void print_names(FILE *file) {
	fprintf(file, "const char *syscall_names[] = {\n");
	for (int i = 0; i < NUM_SYSCALLS; i++) {
		fprintf(
			file, "\t[NG_%s] = \"%s\",\n", syscalls[i].name, syscalls[i].name);
	}
	fprintf(file, "};\n");
}

void print_pointer_masks(FILE *file) {
	fprintf(file, "const int pointer_masks[] = {\n");
	for (int i = 0; i < NUM_SYSCALLS; i++) {
		int mask = 0;
		for (int j = 0; j < syscalls[i].nargs; j++) {
			if (arg_is_pointer(&syscalls[i].args[j])) {
				mask |= 1 << j;
			}
		}
		fprintf(file, "\t[NG_%s] = %d,\n", syscalls[i].name, mask);
	}
	fprintf(file, "};\n");
}

void print_kernel_prototypes(FILE *file) {
	fprintf(file, "\n");
	for (int i = 0; i < NUM_SYSCALLS; i++) {
		fprintf(file, "%s sys_%s(", syscalls[i].return_type, syscalls[i].name);
		if (syscalls[i].frame) {
			fprintf(file, "interrupt_frame *");
			if (syscalls[i].nargs > 0) {
				fprintf(file, ", ");
			}
		}
		for (int j = 0; j < syscalls[i].nargs; j++) {
			fprintf(file, "%s %s", syscalls[i].args[j].type,
				   syscalls[i].args[j].name);
			if (j < syscalls[i].nargs - 1) {
				fprintf(file, ", ");
			}
		}
		fprintf(file, ");\n");
	}
}

void print_switch_case(FILE *file) {
	fprintf(file, "switch (syscall_number) {\n");
	for (int i = 0; i < NUM_SYSCALLS; i++) {
		fprintf(file, "case NG_%s:\n", syscalls[i].name);
		fprintf(file, "\treturn (intptr_t)sys_%s(", syscalls[i].name);
		if (syscalls[i].frame) {
			fprintf(file, "frame");
			if (syscalls[i].nargs > 0) {
				fprintf(file, ", ");
			}
		}
		for (int j = 0; j < syscalls[i].nargs; j++) {
			fprintf(file, "(%s)arg%d", syscalls[i].args[j].type, j);
			if (j < syscalls[i].nargs - 1) {
				fprintf(file, ", ");
			}
		}
		fprintf(file, ");\n");
	}
	fprintf(file, "}\n");
}

void print_strace_prints(FILE *file) {
	fprintf(file, "switch (syscall_number) {\n");
	for (int i = 0; i < NUM_SYSCALLS; i++) {
		fprintf(file, "case NG_%s:\n", syscalls[i].name);
		fprintf(file, "\tprintf(\"%s(", syscalls[i].name);
		for (int j = 0; j < syscalls[i].nargs; j++) {
			fprintf(file, "%s", get_format(&syscalls[i].args[j]));
			if (j < syscalls[i].nargs - 1) {
				fprintf(file, ", ");
			}
		}
		fprintf(file, ")\\n\", ");
		for (int j = 0; j < syscalls[i].nargs; j++) {
			fprintf(file, "(%s)arg%d", syscalls[i].args[j].type, j);
			if (j < syscalls[i].nargs - 1) {
				fprintf(file, ", ");
			}
		}
		fprintf(file, ");\n");
		fprintf(file, "\tbreak;\n");
	}
	fprintf(file, "}\n");
}

void print_user_prototypes(FILE *file) {
	fprintf(file, "#pragma once\n");
	fprintf(file, "#include <ng/syscall_consts.h>\n");
	fprintf(file, "#include <stdint.h>\n");
	fprintf(file, "#include <sys/types.h>\n");
	fprintf(file, "\n");
	for (int i = 0; i < NUM_SYSCALLS; i++) {
		fprintf(file, "%s __ng_%s(", syscalls[i].return_type, syscalls[i].name);
		for (int j = 0; j < syscalls[i].nargs; j++) {
			fprintf(file, "%s %s", syscalls[i].args[j].type,
				syscalls[i].args[j].name);
			if (j < syscalls[i].nargs - 1) {
				fprintf(file, ", ");
			}
		}
		fprintf(file, ");\n");
	}
}

/*
 * Generate a convenience function for each syscall that calls the syscall
 * with the appropriate arguments. For example, for the `foo` syscall:
 *
 * int __ng_foo(int a, int b) {
 *     intptr_t ret = (intptr_t)syscall2(NG_foo, a, b);
 *     if (ret < 0 && ret > -4096) {
 *         errno = -ret;
 *     }
 *     return (int)ret;
 * }
 * __attribute__((weak, alias("__ng_foo"))) typeof(__ng_foo) foo;
 *
 */
void print_user_stubs(FILE *file) {
	fprintf(file, "#include <stdint.h>\n");
	fprintf(file, "#include <errno.h>\n");
	fprintf(file, "#include <ng/syscall.h>\n");
	fprintf(file, "\n");
	for (int i = 0; i < NUM_SYSCALLS; i++) {
		fprintf(file, "%s __ng_%s(", syscalls[i].return_type, syscalls[i].name);
		for (int j = 0; j < syscalls[i].nargs; j++) {
			fprintf(file, "%s %s", syscalls[i].args[j].type,
				syscalls[i].args[j].name);
			if (j < syscalls[i].nargs - 1) {
				fprintf(file, ", ");
			}
		}
		fprintf(file, ") {\n");
		fprintf(file, "\tintptr_t ret = (intptr_t)syscall%d(NG_%s",
			syscalls[i].nargs, syscalls[i].name);
		for (int j = 0; j < syscalls[i].nargs; j++) {
			fprintf(file, ", %s", syscalls[i].args[j].name);
		}
		fprintf(file, ");\n");
		fprintf(file, "\tif (ret < 0 && ret > -4096) {\n");
		fprintf(file, "\t\terrno = -ret;\n");
		if (strchr(syscalls[i].return_type, '*') != NULL) {
			fprintf(file, "\t\treturn NULL;\n");
		} else if (strstr(syscalls[i].return_type, "void") != NULL) {
			fprintf(file, "\t\treturn;\n");
		} else {
			fprintf(file, "\t\treturn (%s)-1;\n", syscalls[i].return_type);
		}
		fprintf(file, "\t}\n");
		fprintf(file, "\treturn (%s)ret;\n", syscalls[i].return_type);
		fprintf(file, "}\n");
		fprintf(file,
			"__attribute__((weak, alias(\"__ng_%s\"))) typeof(__ng_%s) %s;\n",
			syscalls[i].name, syscalls[i].name, syscalls[i].name);
		if (i < NUM_SYSCALLS - 1) {
			fprintf(file, "\n");
		}
	}
}

// report all types used in the syscall table
// so they can be added to the includes.
void print_all_distinct_types() {
	for (int i = 0; i < NUM_SYSCALLS; i++) {
		printf("%s\n", syscalls[i].return_type);
		for (int j = 0; j < syscalls[i].nargs; j++) {
			printf("%s\n", syscalls[i].args[j].type);
		}
	}
}

int main() {
	for (int i = 0; i < NUM_SYSCALLS; i++) {
		if (syscalls[i].nargs > 6) {
			fprintf(stderr, "error: syscall %s has too many arguments\n",
				syscalls[i].name);
			return 1;
		}

		if (syscalls[i].id >= SYSCALL_TABLE_SIZE) {
			fprintf(stderr, "error: syscall %s has id %d, which is too large\n",
				syscalls[i].name, syscalls[i].id);
			return 1;
		}
	}

	FILE *kernel_header = fopen("ng_intf_k.h", "w");
	if (kernel_header == NULL) {
		perror("fopen");
		return 1;
	}
	FILE *user_header = fopen("ng_intf_u.h", "w");
	if (user_header == NULL) {
		perror("fopen");
		return 1;
	}
	FILE *kernel_c = fopen("ng_intf_k.c", "w");
	if (kernel_c == NULL) {
		perror("fopen");
		return 1;
	}
	FILE *user_c = fopen("ng_intf_u.c", "w");
	if (user_c == NULL) {
		perror("fopen");
		return 1;
	}

	fprintf(user_header, "#pragma once\n");
	fprintf(user_header, "#include <sys/cdefs.h>\n");
	fprintf(user_header, "#include <sys/types.h>\n");
	fprintf(user_header, "BEGIN_DECLS\n");
	print_enum(user_header);
	print_user_prototypes(user_header);
	fprintf(user_header, "END_DECLS\n");

	fprintf(kernel_header, "#pragma once\n");
	fprintf(kernel_header, "#include <sys/types.h>\n");
	print_enum(kernel_header);
	print_pointer_masks(kernel_header);
	print_kernel_prototypes(kernel_header);

	print_switch_case(kernel_c);
	print_strace_prints(kernel_c);

	print_user_stubs(user_c);

	return 0;
}
