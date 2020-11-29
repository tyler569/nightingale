#include <basic.h>
#include <elf.h>

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

    return (struct args_size){arg_count, string_len};
}

static size_t partial_copy_args(char **addrs, char *strings, char *const args[],
                                size_t count) {
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

char *const *exec_copy_args(char *out[], char *const args[]) {
    struct args_size size = size_args(args);
    if (!out) out = malloc((size.count + 1) * sizeof(char *) + size.strlen);
    char *strings = (char *)out + (size.count + 1) * sizeof(char *);
    partial_copy_args(out, strings, args, size.count);
    out[size.count] = 0;
    return out;
}

//  loading   ----------------------------------------------------------------

void exec_load_elf(elf_md *elf) {}

sysret exec_load_file(struct file *file) {
    if (file->filetype != FT_BUFFER) return -ENODEV; // ??
    struct membuf_file *membuf_file = (struct membuf_file *)file;
    Elf_Ehdr *elf = membuf_file->memory;
    if (!elf_verify(elf)) return -ENOEXEC; // ?
    elf_md *e = elf_parse(elf);            // LEAKS
    if (!e) return -ENOEXEC;               // ?
    e->file_size = file->len;

    exec_load_elf(e);
    return 0;
}

sysret exec_load_interpreter(const char *name, char *const argv[]) {
    struct file *file = fs_path(name);
    if (!file) return -ENOENT;
    exec_load_file(file);
}

/*
 * Clear memory maps and reinitialize the critial ones
 */
void exec_memory_setup(void) {
    for (int i = 0; i < NREGIONS; i++) {
        running_process->mm_regions[i].base = 0;
    }
    user_map(USER_STACK - 0x100000, USER_STACK);
    user_map(USER_ARGV, USER_ARGV + 0x2000);
    user_map(USER_ENVP, USER_ENVP + 0x2000);
    user_map(SIGRETURN_THUNK, SIGRETURN_THUNK + 0x1000);
    memcpy((void *)SIGRETURN_THUNK, signal_handler_return, 0x10);
}

const char *exec_shebang(struct file *file) {
    if (file->filetype != FT_BUFFER) return false;
    struct membuf_file *membuf_file = (struct membuf_file *)node;
    char *buffer = membuf_file->memory;
    if (file->len > 2 && buffer[0] == '#' && buffer[1] == '!') {
        return buffer + 2;
    }
}

const char *exec_interp(struct file *file) {
    const char *ret = NULL;
    if (file->filetype != FT_BUFFER) return NULL;
    struct membuf_file *membuf_file = (struct membuf_file *)node;
    char *buffer = membuf_file->memory;
    if (!elf_verify(elf)) return NULL;
    elf_md *e = elf_parse(elf);
    if (!e) return NULL;
    e->file_size = file->len;

    if (e->image->type != ET_DYN) goto out;
    Elf_Phdr *interp = elf_find_phdr(e, PT_INTERP);
    if (!interp) goto out;
    ret = buffer + inter->p_offset;
out:
    free(e);
    return ret;
}


static void exec_frame_setup(interrupt_frame *frame) {
    memset(frame, 0, sizeof(struct interrupt_frame));

    // TODO: x86ism
    frame->ds = 0x20 | 3;
    frame->cs = 0x18 | 3;
    frame->ss = 0x20 | 3;
    frame->flags = INTERRUPT_ENABLE;

    // on I686, arguments are passed above the initial stack pointer
    // so give them some space.  This may not be needed on other
    // platforms, but it's ok for the moment
    frame->user_sp = USER_STACK - 16;
    frame->bp = USER_STACK - 16;
}

sysret do_execve(struct file *file, struct interrupt_frame *frame,
                 const char *filename, char *const argv[], char *const envp[]) {
    const char *path_tmp;
    if (running_process->pid == 0) panic("cannot execve() the kernel\n");

    // copy args to kernel space so they survive if they point to the old args
    char *const *stored_args = exec_copy_args(NULL, argv);

    exec_memory_setup();
    strncpy(running_process->comm, basename, COMM_SIZE);

    if ((path_tmp = exec_shebang(file))) {
        stored_args[0] = filename;
        file = fs_path("/usr/sh"); // TODO: parse the real shebang
        if (!file) return -ENOENT;
    }

    if ((path_tmp = exec_interp(file))) {
        stored_args[0] = file = fd_path(path_tmp);
        if (!file) return -ENOENT;
    }


    // INVALIDATES POINTERS TO USERSPACE
    elf_load(elf);

    exec_frame_setup(frame);
    running_process->mmap_base = USER_MMAP_BASE;

    char *user_argv[] = (char *[])USER_ARGV;
    exec_copy_args(user_argv, stored_args);

    frame->ip = (uintptr_t)elf->e_entry;
    FRAME_ARGC(frame) = argc(stored_args);
    FRAME_ARGV(frame) = (uintptr_t)user_argv;

    free(stored_args);
    return 0;
}
