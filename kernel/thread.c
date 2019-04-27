
// #define DEBUG
#include <ng/basic.h>
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
#include <arch/cpu.h>
#include <arch/memmap.h>
#include <ds/dmgr.h>
#include <fs/tarfs.h>
#include <ng/fs.h>
#include <stddef.h>
#include <stdint.h>

extern uintptr_t boot_pt_root;

struct list runnable_thread_queue = { .head = NULL, .tail = NULL };

kmutex process_lock = KMUTEX_INIT;
struct dmgr processes;
struct dmgr threads;
struct vector fc_ctx;

struct process proc_zero = {
        .pid = 0,
        .comm = "<nightingale>",
        .vm_root = (uintptr_t)&boot_pt_root,
        .parent = 0,
        .thread_count = 1,
        .blocked_threads = {0},
};

extern char boot_kernel_stack; // boot.asm

struct thread thread_zero = {
        .tid = 0,
        .running = true,
        .strace = false,
        .stack = &boot_kernel_stack,
        .state = THREAD_RUNNING,
        .pid = 0,
};

struct process *running_process = &proc_zero;
struct thread *running_thread = &thread_zero;

void threads_init() {
        DEBUG_PRINTF("init_threads()\n");

        await_mutex(&process_lock);

        dmgr_init(&processes);
        dmgr_init(&threads);

        printf("threads: thread data at %p\n", processes.data);

        dmgr_insert(&processes, &proc_zero);
        dmgr_insert(&threads, &thread_zero);

        release_mutex(&process_lock);
}

static struct interrupt_frame *thread_frame(struct thread *th) {
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

static void set_kernel_stack(void *stack_top) {
        extern uintptr_t *kernel_stack;
        *&kernel_stack = stack_top;
}

static void enqueue_thread(struct thread *th) {
        list_append(&runnable_thread_queue, th);
}

static void enqueue_thread_at_front(struct thread *th) {
        list_prepend(&runnable_thread_queue, th);
}

// currently in boot.asm
uintptr_t read_ip();

// portability!
static void fxsave(fp_ctx *fpctx) {
        // printf("called fxsave with %p\n", fpctx);
#if X86_64
        asm volatile("fxsaveq %0" ::"m"(*fpctx));
#elif I686
        asm volatile("fxsave %0" ::"m"(*fpctx));
#endif
}

static void fxrstor(fp_ctx *fpctx) {
        // printf("called fxrstor with %p\n", fpctx);
#if X86_64
        asm volatile("fxrstorq %0" : "=m"(*fpctx));
#elif I686
        asm volatile("fxrstor %0" : "=m"(*fpctx));
#endif

}

struct thread *next_runnable_thread(struct list *q) {
        while (true) { 
                struct thread *to = list_pop_front(q);
                if (!to)  return NULL;

                // thread exitted already
                // need to save this memory somewhere to clean it up.
                if (to->exit_status)  continue;
                if (to->state != THREAD_RUNNING)  continue;
                
                struct process *proc = dmgr_get(&processes, to->pid);

                assert(proc, "bad pid!");

                // process exitted already
                // need to save this memory somewhere to clean it up.
                if (proc->exit_status)  continue;

                return to;
        }
}

void switch_thread(int reason) {
        if (!try_acquire_mutex(&process_lock)) {
                // printf("blocked by the process lock\n");
                interrupt_in_ns(1000);
                return;
        }

        struct thread *to = next_runnable_thread(&runnable_thread_queue);
        // DEBUG_PRINTF("trying %p / %s\n", to, to ? to->tid : -1);

        if (to) {
                if (running_thread->tid != 0 &&
                    running_thread->state == THREAD_RUNNING) {

                        enqueue_thread(running_thread);
                }
        } else {
                if (reason != SW_BLOCK &&
                    running_thread->state == THREAD_RUNNING) {
                        // this thread ran out of time, but no one else is ready
                        // to be run so give it another time slot.

                        interrupt_in_ns(10000); // give process max 10ms to run
                        release_mutex(&process_lock);
                        return;
                } else {
                        to = &thread_zero;
                }
        }

        DEBUG_PRINTF("[am %i, to %i]\n", running_thread->tid, to->tid);

        struct process *to_proc = dmgr_get(&processes, to->pid);
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
                // task switch completed and we have returned to this one
                interrupt_in_ns(10000); // give process max 10ms to run
                release_mutex(&process_lock);
                return;
        }
        running_thread->ip = ip;

        running_process = to_proc;
        running_thread = to;

        interrupt_in_ns(10000); // give process max 10ms to run
        release_mutex(&process_lock);

#if X86_64
        asm volatile(
                "mov %0, %%rbx\n\t"
                "mov %1, %%rsp\n\t"
                "mov %2, %%rbp\n\t"

                // This makes read_ip return 0x99 when we switch back to it
                "mov $0x99, %%rax\n\t"

                "jmp *%%rbx"
                :
                : "r"(to->ip), "r"(to->sp), "r"(to->bp)
                : "%rbx", "%rsp", "%rax"
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
                : "%ebx", "%esp", "%eax"
        );
#endif
}

static void *new_kernel_stack() {
        static char *this_stack = NULL;
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

void new_kernel_thread(uintptr_t entrypoint) {
        DEBUG_PRINTF("new_kernel_thread(%#lx)\n", entrypoint);
        struct thread *th = calloc(1, sizeof(struct thread));

        int new_tid = dmgr_insert(&threads, th);

        memset(th, 0, sizeof(struct thread));

        await_mutex(&process_lock);

        th->stack = (char *)new_kernel_stack() - 8;

        th->tid = new_tid;
        th->bp = th->stack;
        th->sp = th->bp;
        th->ip = entrypoint;
        th->pid = 0;

        struct interrupt_frame *frame = thread_frame(th);
        memset(frame, 0, sizeof(struct interrupt_frame));

        frame_set(frame, FLAGS, INTERRUPT_ENABLE);

        proc_zero.thread_count += 1;
        release_mutex(&process_lock);

        th->state = THREAD_RUNNING;

        enqueue_thread(th);
}

void return_from_interrupt();

void new_user_process(uintptr_t entrypoint) {
        DEBUG_PRINTF("new_user_process(%#lx)\n", entrypoint);
        struct process *proc = malloc(sizeof(struct process));
        struct thread *th = calloc(1, sizeof(struct thread));

        memset(proc, 0, sizeof(struct process));
        memset(th, 0, sizeof(struct thread));

        proc->pid = -1;
        proc->comm = "<init>";
        proc->parent = 0;
        proc->thread_count = 1;
        proc->refcnt = 1;
        proc->mmap_base = USER_MMAP_BASE;

        await_mutex(&process_lock);
        pid_t pid = dmgr_insert(&processes, proc);
        int new_tid = dmgr_insert(&threads, th);
        running_process = proc;

        proc->pid = pid;
        vec_init(&proc->fds, size_t);
        vec_push_value(&proc->fds, 1); // DEV_SERIAL -> stdin (0)
        vec_push_value(&proc->fds, 1); // DEV_SERIAL -> stdout (1)
        vec_push_value(&proc->fds, 1); // DEV_SERIAL -> stderr (2)

        th->tid = new_tid;
        th->stack = (char *)new_kernel_stack() - 8;
        th->bp = th->stack - sizeof(struct interrupt_frame);
        th->sp = th->bp;
        th->ip = (uintptr_t)return_from_interrupt;
        th->pid = proc->pid;
        th->strace = false;

        struct interrupt_frame *frame = thread_frame(th);
        memset(frame, 0, sizeof(*frame));

        frame->ds = 0x18 | 3;
        frame_set(frame, IP, entrypoint);
        frame_set(frame, SP, USER_STACK - 16);
        frame_set(frame, BP, USER_STACK - 16);

        vmm_create_unbacked_range(USER_STACK - 0x100000, 0x100000,
                                  PAGE_USERMODE | PAGE_WRITEABLE);
        vmm_create_unbacked_range(USER_ARGV, 0x2000,
                                  PAGE_USERMODE | PAGE_WRITEABLE);

        // TODO: x86ism
        frame->cs = 0x10 | 3;
        frame->ss = 0x18 | 3;
        frame_set(frame, FLAGS, INTERRUPT_ENABLE);

        proc->vm_root = vmm_fork();
        release_mutex(&process_lock);

        th->state = THREAD_RUNNING;

        enqueue_thread(th);
        switch_thread(SW_BLOCK);
}

noreturn void do_thread_exit(int exit_status, int thread_state) {
        DEBUG_PRINTF("do_thread_exit(%i, %i)\n", exit_status, thread_state);
        running_thread->state = thread_state;
        running_thread->exit_status = exit_status;

        running_process->thread_count -= 1;
        assert(running_process->thread_count >= 0,
               "killed more threads than exist...");

        // save and queue for free()
        dmgr_drop(&threads, running_thread->tid);

        if (running_process->thread_count == 0) {
                running_process->exit_status = exit_status;

                wake_blocked_threads(&running_process->blocked_threads);
                // TODO: signal parent proc of death

                if (!running_process->refcnt) {
                        // save and queue for free()
                        dmgr_drop(&processes, running_process->pid);
                }
        }
        switch_thread(SW_YIELD);

        panic("How even did we get here\n");
}

noreturn struct syscall_ret sys_exit(int exit_status) {
        do_thread_exit(exit_status, THREAD_DONE);
}

noreturn void kill_running_thread(int exit_status) {
        do_thread_exit(exit_status, THREAD_KILLED_FOR_VIOLATION);
}

struct syscall_ret sys_fork(struct interrupt_frame *r) {
        DEBUG_PRINTF("sys_fork(%#lx)\n", r);

        if (running_process->pid == 0) {
                panic("Cannot fork() the kernel\n");
        }

        struct process *new_proc = malloc(sizeof(struct process));
        struct thread *new_th = calloc(1, sizeof(struct thread));

        memset(new_th, 0, sizeof(struct thread));

        memcpy(new_proc, running_process, sizeof(struct process));

        new_proc->pid = -1;
        new_proc->thread_count = 1;

        // copy files to child
        vec_init_copy(&new_proc->fds, &running_process->fds);

        await_mutex(&process_lock);
        pid_t new_pid = dmgr_insert(&processes, new_proc);
        int new_tid = dmgr_insert(&threads, new_th);
        running_process = new_proc;
        new_proc->pid = new_pid;

        new_th->tid = new_tid;
        new_th->stack = new_kernel_stack();
#if I686
        // see below
        new_th->stack -= 4;
#endif
        new_th->bp = new_th->stack - sizeof(struct interrupt_frame);
        new_th->sp = new_th->bp;
        new_th->ip = (uintptr_t)return_from_interrupt;
        new_th->pid = new_pid;
        new_th->strace = running_thread->strace;

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

        new_proc->vm_root = vmm_fork();
        new_th->state = THREAD_RUNNING;

        enqueue_thread(new_th);

        struct syscall_ret ret = {new_proc->pid, 0};
        release_mutex(&process_lock);

        return ret;
}

struct syscall_ret sys_clone0(struct interrupt_frame *r, int (*fn)(void *), 
                              void *new_stack, void *arg, int flags) {
        DEBUG_PRINTF("sys_clone0(%#lx, %p, %p, %p, %i)\n",
                        r, fn, new_stack, arg, flags);

        if (running_process->pid == 0) {
                panic("Cannot clone() the kernel\n");
        }

        struct thread *new_th = calloc(1, sizeof(struct thread));

        memset(new_th, 0, sizeof(struct thread));

        await_mutex(&process_lock);
        int new_tid = dmgr_insert(&threads, new_th);
        new_th->tid = new_tid;
        new_th->stack = new_kernel_stack();
#if I686
        // see below
        new_th->stack -= 4;
#endif
        new_th->bp = new_th->stack - sizeof(struct interrupt_frame);
        new_th->sp = new_th->bp;
        new_th->ip = (uintptr_t)return_from_interrupt;
        new_th->pid = running_process->pid;
        new_th->strace = running_thread->strace;

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

        new_th->state = THREAD_RUNNING;

        enqueue_thread(new_th);

        struct syscall_ret ret = {new_th->tid, 0};
        release_mutex(&process_lock);

        return ret;
}

struct syscall_ret sys_getpid() {
        struct syscall_ret ret = {running_process->pid, 0};
        return ret;
}

struct syscall_ret sys_gettid() {
        struct syscall_ret ret = {running_thread->tid, 0};
        return ret;
}

extern struct tar_header *initfs;

struct syscall_ret sys_execve(struct interrupt_frame *frame, char *filename,
                              char **argv, char **envp) {

        DEBUG_PRINTF("sys_execve(stuff)\n");

        if (running_process->pid == 0) {
                panic("cannot execve() the kernel\n");
        }

        struct syscall_ret ret = {0, 0};

        // LEAKS
        char *new_comm = malloc(strlen(filename));
        strcpy(new_comm, filename);
        running_process->comm = new_comm;

        void *file = tarfs_get_file(initfs, filename);

        if (!file)  RETURN_ERROR(ENOENT);
        Elf *elf = file;
        if (!elf_verify(elf))  RETURN_ERROR(ENOEXEC);

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

        // pretty sure I shouldn't use the environment area for argv...
        char *argument_data = (void *)USER_ENVP;
        char **user_argv = (void *)USER_ARGV;

        size_t argc = 0;
        user_argv[argc++] = argument_data;
        while (*argv) {
                user_argv[argc++] = argument_data;
                argument_data = strcpy(argument_data, *argv);
                argument_data += 1;
                argv += 1;
        }

        frame_set(frame, ARGC, argc);
        frame_set(frame, ARGV, (uintptr_t)user_argv);

        return ret; // value goes nowhere since ip moved.
}

struct syscall_ret sys_wait4(pid_t process) {
        //
        // I misunderstood this syscall
        // this is actually a standard thing and I implemented a
        // very minimal version of it that is far from compatible
        // with the real one.
        //
        // I will likely fix this in the future, for the moment
        // I am disabling the syscall that actually calls this and
        // marking it deprecated.  The functionality is replaced by
        // sys_waitpid.  At some point in the future this will be
        // updated for compatability with the real wait4.
        //
        struct syscall_ret ret = {0, 0};
        while (true) {
                await_mutex(&process_lock);
                struct process *proc = dmgr_get(&processes, process);

                if (proc->thread_count) {
                        release_mutex(&process_lock);
                        asm volatile("hlt");
                        continue;
                }
                ret.value = proc->exit_status;
                release_mutex(&process_lock);

                return ret;
                // could this be structured better?
        }
}

struct syscall_ret sys_waitpid(pid_t process, int *status, int options) {
        if (process <= 0) {
                // TODO support 'any child' or groups
                RETURN_ERROR(EINVAL);
        }
        struct process *proc = dmgr_get(&processes, process);
        if (!proc) {
                RETURN_ERROR(ECHILD);
        }
        proc->refcnt++;

        // TODO: permissions checking.
        while (proc->thread_count) {
                if (options & WNOHANG)
                        RETURN_VALUE(0);

                block_thread(&proc->blocked_threads);
        }
        assert(proc, "proc was destroyed early");
        *status = proc->exit_status;

        proc->refcnt--;
        if (!proc->refcnt) {
                vec_free(&proc->fds);
                free(dmgr_drop(&processes, process));
        }

        RETURN_VALUE(process);
}

struct syscall_ret sys_strace(bool enable) {
        struct syscall_ret ret = {0, 0};
        running_thread->strace = enable;
        return ret;
}

void block_thread(struct list *blocked_threads) {
        DEBUG_PRINTF("** block %i\n", running_thread->tid);
        // printf("** block %i\n", running_thread->tid);

        running_thread->state = THREAD_BLOCKED;
        list_append(blocked_threads, running_thread);

        // whoever sets the thread blocking is responsible for bring it back
        switch_thread(SW_BLOCK);
}

static void wake_blocked_thread(void *th_) {
        struct thread *th = th_;
        DEBUG_PRINTF("** wake %i\n", th->tid);

        th->state = THREAD_RUNNING;
        enqueue_thread(th);
}

void wake_blocked_threads(struct list *blocked_threads) {
        if (blocked_threads->head) {
                list_foreach(blocked_threads, wake_blocked_thread);
                switch_thread(SW_YIELD);
        }
}

struct syscall_ret sys_yield(void) {
        switch_thread(SW_YIELD);
        RETURN_VALUE(0);
}

struct syscall_ret sys_setpgid(void) {
        running_process->pgid = running_process->pid;
        RETURN_VALUE(0);
}

static void _kill_this_pgid(void *process) {
        struct process *proc = process;

        if (proc == running_process) {
                return;
        }

        if (proc->pgid == running_process->pgid) {
                // threads will not schedule if their process has an exit status
                //
                // How does this interact with SIGCHLD?
                // *shrugs*
                proc->exit_status = 0xFFFF;
        }
}

struct syscall_ret sys_exit_group(int exit_status) {
        dmgr_foreach(&processes, _kill_this_pgid);
        running_process->exit_status = exit_status;
        do_thread_exit(exit_status, THREAD_DONE);
}

void _print_process(void *process) {
        struct process *proc = process;

        if (proc->exit_status == 0) {
                printf("pid %i: %s\n", proc->pid, proc->comm);
        } else {
                printf("pid %i: %s (defunct: %i (%x))\n", proc->pid, proc->comm,
                                proc->exit_status, proc->exit_status);
        }
}

struct syscall_ret sys_top(int show_threads) {
        if (!show_threads)
                dmgr_foreach(&processes, _print_process);
        RETURN_VALUE(0);
}

