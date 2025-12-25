#include <ctype.h>
#include <elf.h>
#include <errno.h>
#include <ng/fs.h>
#include <ng/memmap.h>
#include <ng/string.h>
#include <ng/syscall.h>
#include <ng/thread.h>
#include <ng/vmo.h>
#include <stdlib.h>

//  argument and environment passing (System V ABI stack layout) ------------

struct args_size {
	size_t count;
	size_t strlen;
};

static struct args_size size_string_array(char *const args[]) {
	size_t i = 0, arg_count, string_len = 0;
	while (args[i]) {
		string_len += strlen(args[i]) + 1;
		i += 1;
	}
	arg_count = i;

	return (struct args_size) { arg_count, string_len };
}

static size_t partial_copy_string_array(
	char **addrs, char *strings, char *const args[], size_t count) {
	size_t str_offset = 0;
	for (size_t i = 0; i < count; i++) {
		strcpy(strings + str_offset, args[i]);
		addrs[i] = strings + str_offset;
		str_offset += strlen(args[i]) + 1;
	}
	return str_offset;
}

char *const *exec_concat_args(char *const a1[], char *const a2[]) {
	struct args_size s1 = size_string_array(a1);
	struct args_size s2 = size_string_array(a2);
	size_t arg_count = s1.count + s2.count;
	size_t string_len = s1.strlen + s2.strlen;

	char **out = malloc((arg_count + 1) * sizeof(char *) + string_len);
	char *strings = (char *)out + (arg_count + 1) * sizeof(char *);

	size_t string_offset = partial_copy_string_array(out, strings, a1, s1.count);
	partial_copy_string_array(out + s1.count, strings + string_offset, a2, s2.count);
	out[arg_count] = 0;
	return out;
}

char *const *exec_copy_args(char **out, char *const args[]) {
	if (!args)
		return nullptr;
	struct args_size size = size_string_array(args);
	if ((size.count + 1) * sizeof(char *) + size.strlen > 0x2000) {
		printf("WARN: argv too large (%zu bytes), truncating\n",
			(size.count + 1) * sizeof(char *) + size.strlen);
		return nullptr;
	}
	if (!out)
		out = malloc((size.count + 1) * sizeof(char *) + size.strlen);
	char *strings = (char *)out + (size.count + 1) * sizeof(char *);
	partial_copy_string_array(out, strings, args, size.count);
	out[size.count] = 0;
	return out;
}

char *const *exec_copy_envp(char **out, char *const envp[]) {
	if (!envp)
		return nullptr;
	struct args_size size = size_string_array(envp);
	if ((size.count + 1) * sizeof(char *) + size.strlen > 0x2000) {
		printf("WARN: envp too large (%zu bytes), truncating\n",
			(size.count + 1) * sizeof(char *) + size.strlen);
		return nullptr;
	}
	if (!out)
		out = malloc((size.count + 1) * sizeof(char *) + size.strlen);
	char *strings = (char *)out + (size.count + 1) * sizeof(char *);
	partial_copy_string_array(out, strings, envp, size.count);
	out[size.count] = 0;
	return out;
}

size_t envc(char *const envp[]) {
	if (!envp)
		return 0;
	size_t i;
	for (i = 0; envp[i]; i++) { }
	return i;
}

/*
 * Takes a string of format
 * "abc foo bar"
 * and chops it into 0-terminated C strings for each whitespace-delimited
 * token of the input. Places up to `len` pointers in `addrs`, reading
 * at most `str_len` from `str`
 *
 * This is destructive to the input string, it is mutated to add the NULs,
 * and the resultant pointers are into that buffer. This is best applied to
 * a temporary and then copied out with exec_copy_args or exec_concat_args.
 */
size_t exec_parse_args(char **addrs, size_t len, char *str, size_t str_len) {
	size_t arg_i = 0;
	for (size_t i = 0; str[i] && i < str_len; i++) {
		if (str[i] == ' ')
			str[i] = 0;
		if (i == 0 || (str[i - 1] == 0 && !isspace(str[i]))) {
			if (arg_i >= len)
				return arg_i;
			addrs[arg_i++] = &str[i];
		}
	}
	return arg_i;
}

size_t argc(char *const args[]) {
	if (!args)
		return 0;
	size_t i;
	for (i = 0; args[i]; i++) { }
	return i;
}

//  loading   ----------------------------------------------------------------

elf_md *exec_open_elf(struct vnode *vnode) {
	if (vnode->type != FT_NORMAL)
		return nullptr;
	void *buffer = vnode->data;

	elf_md *e = elf_parse(buffer, vnode->len);
	return e;
}

bool exec_load_elf(elf_md *e, bool image) {
	elf_load(e);
	return 0;
}

/*
 * Clear memory maps and reinitialize the critical ones:
 * - USER_STACK: Process stack (1MB) with guard page
 * Note: argv/envp are placed on stack per System V ABI
 */
void exec_memory_setup() {
	vma_unmap_all(running_process);
	user_map(USER_STACK - 0x100000, USER_STACK);
	// USER_ARGV and USER_ENVP no longer needed - args/envp on stack
}

const char *exec_shebang(struct vnode *vnode) {
	if (vnode->type != FT_NORMAL)
		return nullptr;
	char *buffer = vnode->data;
	if (vnode->len > 2 && buffer[0] == '#' && buffer[1] == '!') {
		return buffer + 2;
	}
	return nullptr;
}

const char *exec_interp(elf_md *e) {
	const Elf_Phdr *interp = elf_find_phdr(e, PT_INTERP);
	if (!interp)
		return nullptr;
	return (char *)e->buffer + interp->p_offset;
}

/**
 * Build System V ABI-compliant stack layout for execve.
 * Layout (high to low): env strings, arg strings, padding, NULL,
 * envp[], NULL, argv[], argc
 *
 * @param args Argument array (NULL-terminated)
 * @param envp Environment array (NULL-terminated)
 * @param out_argc Output parameter for argument count
 * @param out_envc Output parameter for environment count
 * @return New stack pointer (16-byte aligned), or 0 on error
 */
static uintptr_t exec_build_stack(char *const args[], char *const envp[],
	size_t *out_argc, size_t *out_envc) {
	// 1. Calculate sizes
	struct args_size args_size = args ? size_string_array(args)
		: (struct args_size) { 0, 0 };
	struct args_size envp_size = envp ? size_string_array(envp)
		: (struct args_size) { 0, 0 };

	// 2. Calculate total space
	size_t strings_len = args_size.strlen + envp_size.strlen;
	size_t pointers_len =
		(args_size.count + 1 + envp_size.count + 1) * sizeof(char *);
	size_t total_len = sizeof(size_t) + pointers_len + strings_len;
	total_len = (total_len + 15) & ~15; // Align to 16 bytes

	// 3. Check stack limit (leave 64KB for actual stack usage)
	if (total_len > 0xF0000) {
		printf("WARN: argv/envp too large (%zu bytes), exceeds stack\n",
			total_len);
		return 0;
	}

	// 4. Calculate stack pointer (must be 16-byte aligned)
	uintptr_t stack_top = USER_STACK;
	uintptr_t ptr_base = (stack_top - total_len) & ~15UL;

	// 5. Calculate where each section starts
	uintptr_t strings_start = ptr_base + sizeof(size_t) + pointers_len;

	// 6. Copy environment strings
	char *env_strs = (char *)strings_start;
	size_t env_offset = 0;
	for (size_t i = 0; i < envp_size.count; i++) {
		strcpy(env_strs + env_offset, envp[i]);
		env_offset += strlen(envp[i]) + 1;
	}

	// 7. Copy argument strings
	char *arg_strs = env_strs + envp_size.strlen;
	size_t arg_offset = 0;
	for (size_t i = 0; i < args_size.count; i++) {
		strcpy(arg_strs + arg_offset, args[i]);
		arg_offset += strlen(args[i]) + 1;
	}

	// 8. Write argc
	size_t *argc_ptr = (size_t *)ptr_base;
	*argc_ptr = args_size.count;

	// 9. Write argv array
	char **argv_ptr = (char **)(argc_ptr + 1);
	arg_offset = 0;
	for (size_t i = 0; i < args_size.count; i++) {
		argv_ptr[i] = arg_strs + arg_offset;
		arg_offset += strlen(args[i]) + 1;
	}
	argv_ptr[args_size.count] = NULL;

	// 10. Write envp array
	char **envp_ptr = argv_ptr + args_size.count + 1;
	env_offset = 0;
	for (size_t i = 0; i < envp_size.count; i++) {
		envp_ptr[i] = env_strs + env_offset;
		env_offset += strlen(envp[i]) + 1;
	}
	envp_ptr[envp_size.count] = NULL;

	// 11. Set output parameters
	*out_argc = args_size.count;
	*out_envc = envp_size.count;

	return ptr_base;
}

static void exec_frame_setup(interrupt_frame *frame) {
	memset(frame, 0, sizeof(struct interrupt_frame));

	// TODO: x86ism
	frame->ds = 0x20 | 3;
	frame->cs = 0x18 | 3;
	frame->ss = 0x20 | 3;
	frame->flags = INTERRUPT_ENABLE;

	// user_sp and bp will be set after building stack layout
}

sysret do_execve(struct dentry *dentry, struct interrupt_frame *frame,
	const char *filename, char *const argv[], char *const envp[]) {
	if (running_process->pid == 0) {
		printf("WARN: an attempt was made to `execve` the kernel. Ignoring!\n");
		return -EINVAL;
	}

	struct vnode *vnode = dentry_vnode(dentry), *interp = nullptr;
	// copy args and envp to kernel space so they survive if they point to the
	// old address space (which will be unmapped during exec)
	const char *path_tmp;
	char *const *stored_args = { 0 };
	char *const *stored_envp = { 0 };
	char interp_buf[256] = { 0 };

	if (!(vnode->mode & USR_EXEC))
		return -ENOEXEC;

	strncpy(running_process->comm, dentry->name, COMM_SIZE);

	if ((path_tmp = exec_shebang(vnode))) {
		/* Script:
		 * #!/bin/a b c
		 *
		 * Invoked as ./script d e f
		 *
		 * Loads real ELF `/bin/a` with ARGV:
		 * { "/bin/a", "b", "c", "./script", "d", "e", "f" }
		 */

		strncpyto(interp_buf, path_tmp, 256, '\n');
		char *interp_args[8] = { 0 };
		exec_parse_args(interp_args, 8, interp_buf, 256);
		stored_args = exec_concat_args(interp_args, argv);
		stored_envp = exec_copy_envp(nullptr, envp);

		dentry = resolve_path(interp_args[0]);
		if (IS_ERROR(dentry))
			return ERROR(dentry);
		vnode = dentry_vnode(dentry);
		if (!vnode)
			return -ENOENT;
	} else {
		stored_args = exec_copy_args(nullptr, argv);
		stored_envp = exec_copy_envp(nullptr, envp);
	}
	if (!stored_args || !stored_args[0]) {
		char *fallback_args[] = { (char *)filename, nullptr };
		stored_args = exec_copy_args(nullptr, fallback_args);
	}
	if (!stored_envp) {
		char *empty_envp[] = { nullptr };
		stored_envp = exec_copy_envp(nullptr, empty_envp);
	}

	{
		char path_buf[sizeof(running_process->exe_path)] = { 0 };
		if (pathname(dentry, path_buf, sizeof(path_buf)) > 0) {
			strncpy(running_process->exe_path, path_buf,
				sizeof(running_process->exe_path));
			running_process->exe_path[sizeof(running_process->exe_path) - 1]
				= 0;
		}
	}

	exec_memory_setup();

	elf_md *e = exec_open_elf(vnode);
	if (!e)
		return -ENOEXEC;
	if (running_process->elf_metadata)
		free(running_process->elf_metadata);
	running_process->elf_metadata = e;

	uintptr_t interp_entry = 0;
	if ((path_tmp = exec_interp(e))) {
		// this one will actually load both /bin/ld-ng.so *and* the real
		// executable file and pass the base address of the real file to
		// the dynamic linker _somehow_. TODO
		printf("[Debug] Loading interpreter: %s\n", path_tmp);
		dentry = resolve_path(path_tmp);
		if (IS_ERROR(dentry))
			return ERROR(dentry);
		interp = dentry_vnode(dentry);
		if (!vnode)
			return -ENOENT;

		elf_md *interp_md = exec_open_elf(interp);
		if (!interp_md)
			return -ENOEXEC;

		bool err = exec_load_elf(interp_md, false);
		if (err)
			return -ENOEXEC;
		interp_entry = (uintptr_t)interp_md->imm_header->e_entry;
	}

	close_all_cloexec_files(running_process);

	if (!interp_entry) {
		// INVALIDATES POINTERS TO USERSPACE
		bool err = exec_load_elf(e, true);
		if (err)
			return -ENOEXEC;
	}

	exec_frame_setup(frame);
	running_process->mmap_base = USER_MMAP_BASE;

	size_t arg_count, env_count;
	uintptr_t user_sp = exec_build_stack(stored_args, stored_envp,
		&arg_count, &env_count);
	if (user_sp == 0) {
		return -E2BIG; // Argument list too long
	}

	if (interp_entry)
		frame->ip = interp_entry;
	else
		frame->ip = (uintptr_t)e->imm_header->e_entry;

	// Set up stack and registers per System V ABI
	frame->user_sp = user_sp;
	frame->bp = user_sp;
	FRAME_ARGC(frame) = arg_count;
	FRAME_ARGV(frame) = user_sp + 8; // Skip past argc
	FRAME_ENVP(frame) = user_sp + 8 + (arg_count + 1) * 8; // Skip argc, argv[], NULL

	free((void *)stored_args);
	free((void *)stored_envp);
	return 0;
}

sysret sys_execveat(struct interrupt_frame *frame, int dir_fd, char *filename,
	char *const argv[], char *const envp[]) {
	struct dentry *dentry = resolve_atpath(dir_fd, filename, true);
	if (IS_ERROR(dentry))
		return ERROR(dentry);

	struct vnode *vnode = dentry_vnode(dentry);
	if (!vnode)
		return -ENOENT;

	return do_execve(dentry, frame, filename, argv, envp);
}

sysret sys_execve(struct interrupt_frame *frame, char *filename,
	char *const argv[], char *const envp[]) {
	return sys_execveat(frame, AT_FDCWD, filename, argv, envp);
}
