#include <ctype.h>
#include <elf.h>
#include <errno.h>
#include <ng/arch-2.h>
#include <ng/fs.h>
#include <ng/memmap.h>
#include <ng/string.h>
#include <ng/syscall.h>
#include <ng/thread.h>
#include <stdlib.h>

//  argument passing and copying ---------------------------------------------

struct args_size {
	size_t count;
	size_t strlen;
};

static struct args_size size_args(char *const args[]) {
	size_t i = 0, arg_count, string_len = 0;
	while (args[i]) {
		string_len += strlen(args[i]) + 1;
		i += 1;
	}
	arg_count = i;

	return (struct args_size) { arg_count, string_len };
}

static size_t partial_copy_args(
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
	struct args_size s1 = size_args(a1);
	struct args_size s2 = size_args(a2);
	size_t arg_count = s1.count + s2.count;
	size_t string_len = s1.strlen + s2.strlen;

	char **out = malloc((arg_count + 1) * sizeof(char *) + string_len);
	char *strings = (char *)out + (arg_count + 1) * sizeof(char *);

	size_t string_offset = partial_copy_args(out, strings, a1, s1.count);
	partial_copy_args(out + s1.count, strings + string_offset, a2, s2.count);
	out[arg_count] = 0;
	return out;
}

char *const *exec_copy_args(char **out, char *const args[]) {
	if (!args)
		return nullptr;
	struct args_size size = size_args(args);
	if (!out)
		out = malloc((size.count + 1) * sizeof(char *) + size.strlen);
	char *strings = (char *)out + (size.count + 1) * sizeof(char *);
	partial_copy_args(out, strings, args, size.count);
	out[size.count] = 0;
	return out;
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
 * Clear memory maps and reinitialize the critical ones
 */
void exec_memory_setup() {
	for (int i = 0; i < NREGIONS; i++) {
		running_process->mm_regions[i].base = 0;
	}
	user_map(USER_STACK - 0x100000, USER_STACK);
	user_map(USER_ARGV, USER_ARGV + 0x2000);
	user_map(USER_ENVP, USER_ENVP + 0x2000);
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

static void exec_frame_setup(interrupt_frame *frame) {
	new_user_frame(frame, 0, USER_STACK);
}

sysret do_execve(struct dentry *dentry, struct interrupt_frame *frame,
	const char *filename, char *const argv[], char *const envp[]) {
	if (running_process->pid == 0) {
		printf("WARN: an attempt was made to `execve` the kernel. Ignoring!\n");
		return -EINVAL;
	}

	struct vnode *vnode = dentry_vnode(dentry), *interp = nullptr;
	// copy args to kernel space so they survive if they point to the old args
	const char *path_tmp;
	char *const *stored_args = { 0 };
	char interp_buf[256] = { 0 };

	if (!(vnode->mode & USR_EXEC))
		return -ENOEXEC;

	exec_memory_setup();
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

		dentry = resolve_path(interp_args[0]);
		if (IS_ERROR(dentry))
			return ERROR(dentry);
		vnode = dentry_vnode(dentry);
		if (!vnode)
			return -ENOENT;
	} else {
		stored_args = exec_copy_args(nullptr, argv);
	}

	elf_md *e = exec_open_elf(vnode);
	if (!e)
		return -ENOEXEC;
	if (running_process->elf_metadata)
		free(running_process->elf_metadata);
	running_process->elf_metadata = e;

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
		if (!err)
			return -ENOEXEC;
	}

	close_all_cloexec_files(running_process);

	// INVALIDATES POINTERS TO USERSPACE
	bool err = exec_load_elf(e, true);
	if (err)
		return -ENOEXEC;

	exec_frame_setup(frame);
	running_process->mmap_base = USER_MMAP_BASE;

	char **user_argv = (char **)USER_ARGV;
	exec_copy_args(user_argv, stored_args);

	// FIXME: it's not e->header->entry if there's an interpreter
	frame->rip = (uintptr_t)e->imm_header->e_entry;
	FRAME_ARGC(frame) = argc(stored_args);
	FRAME_ARGV(frame) = (uintptr_t)user_argv;

	free((void *)stored_args);
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
