
#include <basic.h>
#include <ng/debug.h>
#include <ng/elf.h>
#include <ng/malloc.h>
#include <ng/mutex.h>
#include <ng/panic.h>
#include <ng/print.h>
#include <ng/string.h>
#include <ng/syscall.h>
#include <ng/syscalls.h>
#include <ng/thread.h>
#include <ng/timer.h>
#include <ng/vmm.h>
#include <ng/cpu.h>
#include <ng/memmap.h>
#include <ng/dmgr.h>
#include <ng/tarfs.h>
#include <ng/fs.h>
#include <ng/signal.h>
#include <nc/errno.h>
#include <stddef.h>
#include <stdint.h>

extern uintptr_t boot_pt_root;

struct list runnable_thread_queue = { .head = NULL, .tail = NULL };

noreturn ng_static void do_thread_exit(int exit_status, int thread_state);

kmutex process_lock = KMUTEX_INIT;
struct dmgr processes;
struct dmgr threads;

struct process proc_zero = {
        .pid = 0,
        .comm = "<nightingale>",
        .vm_root = (uintptr_t)&boot_pt_root,
        .parent = NULL,
};

extern char boot_kernel_stack; // boot.asm

struct thread thread_zero = {
        .tid = 0,
        .stack = &boot_kernel_stack,
        .thread_state = THREAD_RUNNING,
};

struct process *running_process = &proc_zero;
struct thread *running_thread = &thread_zero;

/*
struct process *proc_region = NULL;
struct thread *thread_region = NULL;
struct list free_proc_slots = {0};
struct list free_th_slots = {0};
*/

struct process *new_process_slot() {
        /*
        if (free_proc_slots.head) {
                return list_pop_front(&free_proc_slots);
        } else {
                return proc_region++;
        }
        */
        return malloc(sizeof(struct process));
}

struct thread *new_thread_slot() {
        /*
        if (free_th_slots.head) {
                return list_pop_front(&free_th_slots);
        } else {
                return thread_region++;
        }
        */
        return malloc(sizeof(struct thread));
}

void free_process_slot(struct process *defunct) {
        // list_prepend(&free_proc_slots, defunct);
        free(defunct);
}

void free_thread_slot(struct thread *defunct) {
        // list_prepend(&free_th_slots, defunct);
        free(defunct);
}

ng_static int process_matches(pid_t wait_arg, struct process *proc) {
        if (wait_arg == 0) {
                return 1;
        } else if (wait_arg > 0) {
                return wait_arg == proc->pid;
        } else if (wait_arg == -1) {
                return true;
        } else if (wait_arg < 0) {
                return -wait_arg == proc->pgid;
        }
        return 0;
}

struct process *process_by_id(pid_t pid) {
        struct process *p = dmgr_get(&processes, pid);
        return p;
}

struct thread *thread_by_id(pid_t tid) {
        struct thread *th = dmgr_get(&threads, tid);
        return th;
}

void threads_init() {
        DEBUG_PRINTF("init_threads()\n");
        
        /*
        proc_region = vmm_reserve(512 * 1024);
        thread_region = vmm_reserve(1 * 1024*1024);
        */

        dmgr_init(&processes);
        dmgr_init(&threads);

        await_mutex(&process_lock);

        printf("threads: thread data at %p\n", processes.data);

        thread_zero.proc = &proc_zero;
        
        dmgr_insert(&processes, &proc_zero);
        dmgr_insert(&threads, &thread_zero);

        list_append(&proc_zero.threads, &thread_zero);

        release_mutex(&process_lock);
}

ng_static struct interrupt_frame *thread_frame(struct thread *th) {
#if X86_64
        struct interrupt_frame *frame = th->sp;
#elif I686
        // I686 has to push a parameter to the C interrupt shim,
        // so the first thing return_from_interrupt does is restore
        // the stack.  This needs to start 4 higher to be compatible with
        // that ABI.
        struct interrupt_frame *frame = (void *)((char *)th->sp + 4);
#endif
        return frame;
}

void set_kernel_stack(void *stack_top) {
        extern uintptr_t *kernel_stack;
        *&kernel_stack = stack_top;
}

void enqueue_thread(struct thread *th) {
        // Thread 0 is the default and requests to enqueue it should
        // be ignored
        if (th->tid == 0)  return;
        list_append(&runnable_thread_queue, th);
}

void enqueue_thread_at_front(struct thread *th) {
        // Thread 0 is the default and requests to enqueue it should
        // be ignored
        if (th->tid == 0)  return;
        list_prepend(&runnable_thread_queue, th);
}

void drop_thread(struct thread *th) {
        if (th->tid == 0)  return;
        list_remove(&runnable_thread_queue, th);
}

// currently in boot.asm
__attribute__((returns_twice))
extern uintptr_t read_ip(void);

// portability!
ng_static void fxsave(fp_ctx *fpctx) {
        // printf("called fxsave with %p\n", fpctx);
#if X86_64
        asm volatile("fxsaveq %0" ::"m"(*fpctx));
#elif I686
        asm volatile("fxsave %0" ::"m"(*fpctx));
#endif
}

ng_static void fxrstor(fp_ctx *fpctx) {
        // printf("called fxrstor with %p\n", fpctx);
#if X86_64
        asm volatile("fxrstorq %0" : "=m"(*fpctx));
#elif I686
        asm volatile("fxrstor %0" : "=m"(*fpctx));
#endif

}

struct thread *next_runnable_thread(struct list *q) {
        return list_pop_front(q);
}

struct thread *peek_runnable_thread(struct list *q) {
        return list_head(q);
}

struct thread *peek_rtq_dbg() {
        return list_head(&runnable_thread_queue);
}

#define PROC_RUN_NS 5000

#define SW_TAKE_LOCK 0

struct thread garbage;

void switch_thread(int reason) {
        struct thread *to;
        struct process *to_proc;

        if (reason == SW_REQUEUE) {
                to = running_thread;
                to_proc = running_process;
                set_kernel_stack(to->stack); // maybe needed - unknown
                goto skip_save_state;
        }

        to = next_runnable_thread(&runnable_thread_queue);

        /*
         * when switch_thread is called with SW_DONE, the running thread
         * has already been deallocated. Accessing it would be a use
         * after free. We need to do something in this function to prevent
         * that, so I have it use a stack variable.
         */
        if (reason == SW_DONE)  running_thread = &garbage;
        if (to && running_thread->thread_state == THREAD_RUNNING) {
                enqueue_thread(running_thread);
        }
        if (!to && running_thread->thread_state == THREAD_RUNNING) {
                // this thread ran out of time, but no one else is ready
                // to be run so give it another time slot.
                interrupt_in_ns(PROC_RUN_NS);
                return;
        }

        if (!to)  to = &thread_zero;

        DEBUG_PRINTF("[am %i, to %i]\n", running_thread->tid, to->tid);

        to_proc = to->proc;

        set_kernel_stack(to->stack);
        if (running_process->pid != 0) {
                fxsave(&running_thread->fpctx);
        }
        if (to_proc->pid != 0) {
                set_vm_root(to_proc->vm_root);
                fxrstor(&to->fpctx);
        }

#if X86_64
        asm volatile("mov %%rsp, %0" : "=r"(running_thread->sp));
        asm volatile("mov %%rbp, %0" : "=r"(running_thread->bp));
#elif I686
        asm volatile("mov %%esp, %0" : "=r"(running_thread->sp));
        asm volatile("mov %%ebp, %0" : "=r"(running_thread->bp));
#endif

        uintptr_t ip = read_ip();

        if (ip == 0x99) {
                if (running_process->signal_pending) {
                        handle_pending_signal();
                }

                if (running_thread->thread_state == THREAD_KILLED) {
                        do_thread_exit(0, THREAD_DONE);
                }

                interrupt_in_ns(PROC_RUN_NS);
                return;
        }

        running_thread->ip = ip;

        running_process = to_proc;
        running_thread = to;

skip_save_state:

        interrupt_in_ns(PROC_RUN_NS);

#if X86_64
        asm volatile(
                "mov %0, %%rbx\n\t"
                "mov %1, %%rsp\n\t"
                "mov %2, %%rbp\n\t"

                // This makes read_ip return 0x99 when we switch back to it
                "mov $0x99, %%rax\n\t"

                "jmp *%%rbx"
                :
                : "b"(to->ip), "c"(to->sp), "d"(to->bp)
                : "%rax", "%rsi", "%rdi", "%r8", "%r9", "%r10",
                  "%r11", "%r12", "%r13", "%r14", "%r15", "memory"
        );
#elif I686
        asm volatile(
                "mov %0, %%ebx\n\t"
                "mov %1, %%esp\n\t"
                "mov %2, %%ebp\n\t"

                // This makes read_ip return 0x99 when we switch back to it
                "mov $0x99, %%eax\n\t"

                "jmp *%%ebx"
                :
                : "r"(to->ip), "r"(to->sp), "r"(to->bp)
                : "%ebx", "%eax"
        );
#endif
}

ng_static void *new_kernel_stack() {
        ng_static char *this_stack = NULL;
        if (!this_stack)  this_stack = vmm_reserve(4096 * 1024);

        // leave 1 page unmapped for guard
        vmm_edit_flags((uintptr_t)this_stack, PAGE_STACK_GUARD);
        this_stack += PAGE_SIZE;
        // 8k stack
        vmm_create((uintptr_t)this_stack, PAGE_WRITEABLE);
        this_stack += PAGE_SIZE;
        vmm_create((uintptr_t)this_stack, PAGE_WRITEABLE);
        this_stack += PAGE_SIZE;
        DEBUG_PRINTF("new kernel stack at (top): %p\n", this_stack);
        return this_stack;
}

void new_kthread(uintptr_t entrypoint) {
        DEBUG_PRINTF("new_kernel_thread(%#lx)\n", entrypoint);

        struct thread *th = new_thread_slot();
        int new_tid = dmgr_insert(&threads, th);
        memset(th, 0, sizeof(struct thread));

        await_mutex(&process_lock);

        th->stack = (char *)new_kernel_stack() - 8;

        th->tid = new_tid;
        th->bp = th->stack;
        th->sp = th->bp;
        th->ip = entrypoint;
        th->proc = &proc_zero;

        struct interrupt_frame *frame = thread_frame(th);
        memset(frame, 0, sizeof(struct interrupt_frame));

        frame_set(frame, FLAGS, INTERRUPT_ENABLE);
        list_append(&proc_zero.threads, th);

        release_mutex(&process_lock);

        th->thread_state = THREAD_RUNNING;

        enqueue_thread(th);
}

void new_user_process(uintptr_t entrypoint) {
        DEBUG_PRINTF("new_user_process(%#lx)\n", entrypoint);
        struct process *proc = new_process_slot();
        struct thread *th = new_thread_slot();

        memset(proc, 0, sizeof(struct process));
        memset(th, 0, sizeof(struct thread));

        proc->pid = -1;
        proc->comm = "<init>";
        proc->parent = 0;
        proc->mmap_base = USER_MMAP_BASE;
        proc->parent = running_process;
        list_append(&running_process->children, proc);
        list_append(&proc->threads, th);

        await_mutex(&process_lock);
        int pid = dmgr_insert(&processes, proc);
        int tid = dmgr_insert(&threads, th);

        proc->pid = pid;
        dmgr_init(&proc->fds);
        /*
        This is how done by init to support multiple login shells

        dmgr_insert(&proc->fds, ofd_stdin);
        dmgr_insert(&proc->fds, ofd_stdout);
        dmgr_insert(&proc->fds, ofd_stderr);
        */

        th->tid = tid;
        th->stack = (char *)new_kernel_stack() - 8;
        th->bp = th->stack - sizeof(struct interrupt_frame);
        th->sp = th->bp;
        th->ip = (uintptr_t)return_from_interrupt;
        th->proc = proc;
        // th->thread_flags = THREAD_STRACE;
        th->cwd = fs_resolve_relative_path(fs_root_node, "/bin");

        struct interrupt_frame *frame = thread_frame(th);
        memset(frame, 0, sizeof(*frame));

        frame->ds = 0x18 | 3;
        frame_set(frame, IP, entrypoint);
        frame_set(frame, SP, USER_STACK - 16);
        frame_set(frame, BP, USER_STACK - 16);

        vmm_create_unbacked_range(USER_STACK - 0x100000, 0x100000,
                                  PAGE_USERMODE | PAGE_WRITEABLE);
        vmm_create_unbacked_range(USER_ARGV, 0x20000,
                                  PAGE_USERMODE | PAGE_WRITEABLE);

        // TODO: x86ism
        frame->cs = 0x10 | 3;
        frame->ss = 0x18 | 3;
        frame_set(frame, FLAGS, INTERRUPT_ENABLE);

        proc->vm_root = vmm_fork();
        release_mutex(&process_lock);

        th->thread_state = THREAD_RUNNING;

        enqueue_thread_at_front(th);
        switch_thread(SW_YIELD);
}

void bootstrap_usermode(const char *init_filename) {
        vmm_create_unbacked_range(SIGRETURN_THUNK, 0x1000,
                        PAGE_USERMODE | PAGE_WRITEABLE); // make read-only
        memcpy((void *)SIGRETURN_THUNK, signal_handler_return, 0x10);

        struct fs_node *init =
                fs_resolve_relative_path(fs_root_node, init_filename);
        assert(init); //, "init not found");
        assert(init->filetype == MEMORY_BUFFER);//, "init is not a file");

        Elf *program = init->memory;

        if (!elf_verify(program)) {
                panic("init is not a valid ELF\n");
        }

        elf_load(program);
        printf("Starting ring 3 thread at %#zx\n\n", program->e_entry);
        new_user_process(program->e_entry);

        switch_thread(SW_YIELD);
}

noreturn ng_static void do_thread_exit(int exit_status, int thread_state) {
        DEBUG_PRINTF("do_thread_exit(%i, %i)\n", exit_status, thread_state);
        running_thread->thread_state = thread_state;

        // TODO:
        // this may be fragile to context swaps happening during this fucntion
        // I should consider and remediate that
        
        if (running_thread->blocking_list) {
                list_remove_node(running_thread->blocking_list,
                                 running_thread->blocking_node);
        }
        if (running_thread->blocking_event) {
                drop_timer_event(running_thread->blocking_event);
        }

        struct thread *defunct = dmgr_drop(&threads, running_thread->tid);
        free_thread_slot(defunct);

        list_remove(&running_process->threads, running_thread);

        if (running_process->threads.head) {
                // This thread can be removed from the running queue,
                // as it will never run again.
                // printf("still more threads\b");
                switch_thread(SW_DONE);
        }

        if (running_process->pid == 1) {
                panic("attempted to kill init!");
        }

        if (running_process->pid == 0) {
                switch_thread(SW_DONE);
        }

        running_process->exit_status = exit_status + 1;
        // This was the last thread - need to wait for a wait on the process.

        struct list_n *node = running_process->parent->threads.head;
        for (; node; node = node->next) {
                struct thread *th = node->v;
                // printf("checking thread %i\n", th->tid);
                if ((th->thread_flags & THREAD_WAIT) == 0) {
                        continue;
                }
                if (process_matches(th->wait_request, running_process)) {
                        th->wait_result = running_process;
                        th->thread_state = THREAD_RUNNING;
                        enqueue_thread(th);
                }
        }
        // If we got here, no one is listening currently.
        // someone may call waitpid(2) later, so leave this around and if
        // it doesn't get cleaned up it becomes a zombie.
        //
        // This thread can be removed from the running queue, as it will
        // never run again.
        // printf("waiting for someone to save me\n");

        switch_thread(SW_DONE);

        panic("Thread awoke after being reaped");
}

noreturn sysret sys_exit(int exit_status) {
        do_thread_exit(exit_status, THREAD_DONE);
}

noreturn void exit_kthread() {
        do_thread_exit(0, THREAD_DONE);
}

sysret sys_fork(struct interrupt_frame *r) {
        DEBUG_PRINTF("sys_fork(%#lx)\n", r);

        if (running_process->pid == 0) {
                panic("Cannot fork() the kernel\n");
        }

        struct process *new_proc = new_process_slot();
        struct thread *new_th = new_thread_slot();

        //memcpy(new_proc, running_process, sizeof(struct process));
        memset(new_proc, 0, sizeof(struct process));
        memset(new_th, 0, sizeof(struct thread));

        new_proc->pid = -1;
        new_proc->parent = running_process;
        new_proc->comm = malloc(strlen(running_process->comm));
        strcpy(new_proc->comm, running_process->comm);
        new_proc->pgid = running_process->pgid;
        new_proc->uid = running_process->uid;
        new_proc->gid = running_process->gid;
        new_proc->mmap_base = running_process->mmap_base;

        // copy files to child
        dmgr_copy(&new_proc->fds, &running_process->fds);
        list_append(&running_process->children, new_proc);
        list_append(&new_proc->threads, new_th);

        await_mutex(&process_lock);
        int new_pid = dmgr_insert(&processes, new_proc);
        int new_tid = dmgr_insert(&threads, new_th);
        // running_process = new_proc;
        new_proc->pid = new_pid;

        new_th->tid = new_tid;
        new_th->stack = new_kernel_stack();
        new_th->user_sp = running_thread->user_sp;
#if I686
        // see below
        new_th->stack -= 4;
#endif
        new_th->bp = new_th->stack - sizeof(struct interrupt_frame);
        new_th->sp = new_th->bp;
        new_th->ip = (uintptr_t)return_from_interrupt;
        new_th->proc = new_proc;
        new_th->thread_flags = running_thread->thread_flags;
        new_th->cwd = running_thread->cwd;

        struct interrupt_frame *frame = thread_frame(new_th);
        memcpy(frame, r, sizeof(struct interrupt_frame));
        frame_set(frame, RET_VAL, 0);
        frame_set(frame, RET_ERR, 0);

        new_proc->vm_root = vmm_fork();
        new_th->thread_state = THREAD_RUNNING;

        enqueue_thread(new_th);

        sysret ret = new_proc->pid;
        release_mutex(&process_lock);

        return ret;
}

sysret sys_clone0(struct interrupt_frame *r, int (*fn)(void *), 
                              void *new_stack, void *arg, int flags) {
        DEBUG_PRINTF("sys_clone0(%#lx, %p, %p, %p, %i)\n",
                        r, fn, new_stack, arg, flags);

        if (running_process->pid == 0) {
                panic("Cannot clone() the kernel\n");
        }

        struct thread *new_th = new_thread_slot();

        memset(new_th, 0, sizeof(struct thread));

        await_mutex(&process_lock);
        int new_tid = dmgr_insert(&threads, new_th);
        new_th->tid = new_tid;
        new_th->stack = new_kernel_stack();

        list_append(&running_process->threads, new_th);
#if I686
        // see below
        new_th->stack -= 4;
#endif
        new_th->bp = new_th->stack - sizeof(struct interrupt_frame);
        new_th->sp = new_th->bp;
        new_th->ip = (uintptr_t)return_from_interrupt;
        new_th->proc = running_process;
        new_th->thread_flags = running_thread->thread_flags;
        new_th->cwd = running_thread->cwd;

#if X86_64
        struct interrupt_frame *frame = new_th->sp;
#elif I686
        // I686 has to push a parameter to the C interrupt shim,
        // so the first thing return_from_interrupt does is restore
        // the stack.  This needs to start 4 higher to be compatible with
        // that ABI.
        struct interrupt_frame *frame = (void *)((char *)new_th->sp + 4);
#endif
        memcpy(frame, r, sizeof(struct interrupt_frame));
        frame_set(frame, RET_VAL, 0);
        frame_set(frame, RET_ERR, 0);

        frame_set(frame, SP, (uintptr_t)new_stack);
        frame_set(frame, BP, (uintptr_t)new_stack);
        frame_set(frame, IP, (uintptr_t)fn);

        new_th->thread_state = THREAD_RUNNING;

        enqueue_thread(new_th);

        release_mutex(&process_lock);

        return new_th->tid;
}

sysret sys_getpid() {
        return running_process->pid;
}

sysret sys_gettid() {
        return running_thread->tid;
}

extern struct tar_header *initfs;

sysret do_execve(struct fs_node *node, struct interrupt_frame *frame,
                 char **argv, char **envp) {

        if (running_process->pid == 0) {
                panic("cannot execve() the kernel\n");
        }

        // if (!(node->perms & USR_EXEC))  return -ENOEXEC;

        char *new_comm = malloc(strlen(node->filename));
        strcpy(new_comm, node->filename);
        running_process->comm = new_comm;

        if (!(node->filetype == MEMORY_BUFFER))  return -ENOEXEC;
        void *file = node->memory;
        if (!file)  return -ENOENT;
        Elf *elf = file;
        if (!elf_verify(elf))  return -ENOEXEC;

        // memset(argument_data, 0, 4096);

        // pretty sure I shouldn't use the environment area for argv...
        char *argument_data = (void *)USER_ENVP;
        char **user_argv = (void *)USER_ARGV;
        size_t argc = 0;
        while (*argv) {
                // printf("[DEBUG] : argument is %p:\"%s\"\n", *argv, *argv);
                user_argv[argc] = argument_data;
                argument_data = strcpy(argument_data, *argv) + 1;
                argc += 1;
                argv += 1;
        }
        user_argv[argc] = 0;

        // INVALIDATES POINTERS TO USERSPACE
        elf_load(elf);

        memset(frame, 0, sizeof(struct interrupt_frame));
        // TODO: x86ism
        frame->ds = 0x18 | 3;
        frame->cs = 0x10 | 3;
        frame->ss = 0x18 | 3;
        frame_set(frame, IP, (uintptr_t)elf->e_entry);
        frame_set(frame, FLAGS, INTERRUPT_ENABLE);

        // on I686, arguments are passed above the initial stack pointer
        // so give them some space.  This may not be needed on other
        // platforms, but it's ok for the moment
        frame_set(frame, SP, USER_STACK - 16);
        frame_set(frame, BP, USER_STACK - 16);

        /*
         * MOVED ABOVE
        // pretty sure I shouldn't use the environment area for argv...
        char *argument_data = (void *)USER_ENVP;
        char **user_argv = (void *)USER_ARGV;

        // memset(argument_data, 0, 4096);

        size_t argc = 0;
        while (*argv) {
                printf("[DEBUG] : argument is %p:\"%s\"\n", *argv, *argv);
                user_argv[argc] = argument_data;
                argument_data = strcpy(argument_data, *argv) + 1;
                argc += 1;
                argv += 1;
        }
        user_argv[argc] = 0;
        */

        frame_set(frame, ARGC, argc);
        frame_set(frame, ARGV, (uintptr_t)user_argv);

        return 0;
}

sysret sys_execve(struct interrupt_frame *frame, char *filename,
                              char **argv, char **envp) {
        DEBUG_PRINTF("sys_execve(<frame>, \"%s\", <argv>, <envp>)\n", filename);

        struct fs_node *file = fs_resolve_relative_path(
                        running_thread->cwd, filename);
        if (!file)  return -ENOENT;

        return do_execve(file, frame, argv, envp);
}

sysret sys_execveat(struct interrupt_frame *frame,
                        int dir_fd, char *filename,
                        char **argv, char **envp) {
        struct open_fd *ofd = dmgr_get(&running_process->fds, dir_fd);
        if (!ofd)  return -EBADF;
        struct fs_node *node = ofd->node;
        if (node->filetype != DIRECTORY)  return -EBADF;

        struct fs_node *file = fs_resolve_relative_path(node, filename);
        return do_execve(file, frame, argv, envp);
}

sysret sys_wait4(pid_t process) {
        //
        // I misunderstood this syscall
        // this is actually a standard thing and I implemented a
        // very minimal version of it that is far from compatible
        // with the real one.
        //
        // I will likely fix this in the future, for the moment
        // I am disabling the syscall that actually calls this and
        // marking it deprecated.  The functionality is replaced by
        //
        // sys_waitpid.  At some point in the future this will be
        // updated for compatability with the real wait4.
        //
        return -EPERM;
}

ng_static void move_children_to_init(void *v) {
        struct process *proc = v;
        
        proc->parent = dmgr_get(&processes, 1);
        list_append(&proc->parent->children, proc);
}

ng_static void destroy_child_process(struct process *proc) {
        dmgr_free(&proc->fds);
        list_foreach(&proc->children, move_children_to_init);
        list_free(&proc->children);
        list_free(&proc->threads); // should be empty
        free(proc->comm);
        list_remove(&running_process->children, proc);
        vmm_destroy_tree(proc->vm_root);
        free_process_slot(dmgr_drop(&processes, proc->pid));
}

sysret sys_waitpid(pid_t process, int *status, int options) {
        int exit_code;
        int found_pid;
        int found_candidate = 0;

        struct list_n *node = running_process->children.head;
        for (; node; node = node->next) {
                struct process *p = node->v;

                if (process_matches(process, p)) {
                        found_candidate = 1;
                } else {
                        continue;
                }
                if (p->exit_status > 0) {
                        // can clean up now
                        exit_code = p->exit_status - 1;
                        found_pid = p->pid;
                        destroy_child_process(p);

                        *status = exit_code;
                        return found_pid;
                }
        }

        if (!found_candidate) {
                return -ECHILD;
        }

        if (options & WNOHANG) {
                return 0;
        }

        running_thread->wait_request = process;
        running_thread->wait_result = 0;

        running_thread->thread_state = THREAD_BLOCKED;
        running_thread->thread_flags |= THREAD_WAIT;

        while (running_thread->wait_result == 0) {
                switch_thread(SW_BLOCK);
                // *********** rescheduled when a wait() comes in.
                // see do_thread_exit()
        }

        struct process *p = running_thread->wait_result;
        exit_code = p->exit_status - 1;
        found_pid = p->pid;

        destroy_child_process(p);

        running_thread->wait_request = 0;
        running_thread->wait_result = NULL;
        running_thread->thread_flags &= ~THREAD_WAIT;

        *status = exit_code;
        return found_pid;
}

sysret sys_strace(bool enable) {
        if (enable) {
                running_thread->thread_flags |= THREAD_STRACE;
        } else { 
                running_thread->thread_flags &= ~THREAD_STRACE;
        }
        return enable;
}

void block_thread(struct list *blocked_threads) {
        DEBUG_PRINTF("** block %i\n", running_thread->tid);
        // printf("** block %i\n", running_thread->tid);

        running_thread->blocking_list = blocked_threads;
        running_thread->thread_state = THREAD_BLOCKED;
        struct list_n *node = list_append(blocked_threads, running_thread);
        running_thread->blocking_node = node;

        // whoever sets the thread blocking is responsible for bring it back
        // the blocking_list/node is saved for when something interrupts the
        // thread and it needs to be removed from its blocking queue.
        // i.e. it is killed.
        switch_thread(SW_BLOCK);
}

ng_static void wake_blocked_thread(void *th_) {
        struct thread *th = th_;
        DEBUG_PRINTF("** wake %i\n", th->tid);

        th->thread_state = THREAD_RUNNING;
        th->blocking_list = 0;
        th->blocking_node = 0;

        enqueue_thread(th);
}

void wake_blocked_threads(struct list *blocked_threads) {
        if (blocked_threads->head) {
                list_foreach(blocked_threads, wake_blocked_thread);
                list_free(blocked_threads);
                switch_thread(SW_YIELD);
        }
}

sysret sys_yield(void) {
        switch_thread(SW_YIELD);
        return 0;
}

sysret sys_setpgid(void) {
        running_process->pgid = running_process->pid;
        return 0;
}

sysret sys_exit_group(int exit_status) {
        kill_process_group(running_process->pgid);
        running_process->exit_status = exit_status + 1;
        do_thread_exit(exit_status, THREAD_DONE);
}

void kill_thread(void *thread) {
        struct thread *th = thread;
        if (th == running_thread) {
                do_thread_exit(0, THREAD_KILLED);
        } else {
                th->thread_state = THREAD_KILLED;
                drop_thread(th);
                enqueue_thread_at_front(th);
        }
}

void kill_process(struct process *p) {
        list_foreach(&p->threads, kill_thread);
        switch_thread(SW_YIELD);
}

void kill_pid(pid_t pid) {
        struct process *p = dmgr_get(&processes, pid);
        if (p)  kill_process(p);
}

static void _kill_pg_by_id(void *process, long pgid) {
        struct process *proc = process;
        if (proc->pgid == pgid) {
                list_foreach(&proc->threads, kill_thread);
        }
}

void kill_process_group(pid_t pgid) {
        dmgr_foreachl(&processes, _kill_pg_by_id, pgid);

        switch_thread(SW_YIELD);
}


void _print_thread(void *thread) {
        struct thread *th = thread;

        char *wait = th->thread_flags & THREAD_WAIT ? "W" : " ";

        char *status;
        switch (th->thread_state) {
        case THREAD_RUNNING: status = "r"; break;
        case THREAD_KILLED:  status = "Z"; break;
        default:             status = "B"; break;
        }

        printf("  t: %i %s%s\n", th->tid, wait, status);
}

void _print_process(void *process) {
        struct process *proc = process;

        if (proc->exit_status <= 0) {
                printf("pid %i: %s\n", proc->pid, proc->comm);
        } else {
                printf("pid %i: %s (defunct: %i (%x))\n", proc->pid, proc->comm,
                        proc->exit_status, proc->exit_status);
        }

        list_foreach(&proc->threads, _print_thread);
}

sysret sys_top(int show_threads) {
        if (!show_threads)
                dmgr_foreach(&processes, _print_process);
        return 0;
}

void wake_process_thread(struct process *p) {
        struct thread *th = p->threads.head->v;
        if (!th)  return;
        
        th->thread_state = THREAD_RUNNING;
        // th->thread_flags |= THREAD_WOKEN;

        drop_thread(th);
        enqueue_thread_at_front(th);
        switch_thread(SW_YIELD);
}

void wake_thread_from_sleep(void *thread) {
        struct thread *th = thread;
        th->thread_state = THREAD_RUNNING;
        th->blocking_event = NULL;
        enqueue_thread(th);
}

sysret sys_sleepms(int ms) {
        running_thread->thread_state = THREAD_SLEEP;
        struct timer_event *te = insert_timer_event(
                        milliseconds(ms), wake_thread_from_sleep, running_thread);
        running_thread->blocking_event = te;
        
        switch_thread(SW_BLOCK);
        return 0;
}

