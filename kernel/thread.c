
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
        .parent = NULL,
};

extern char boot_kernel_stack; // boot.asm

struct thread thread_zero = {
        .tid = 0,
        .stack = &boot_kernel_stack,
        .state = THREAD_RUNNING,
};

struct process *running_process = &proc_zero;
struct thread *running_thread = &thread_zero;

struct process *proc_region = NULL;
struct thread *thread_region = NULL;
struct list free_proc_slots = {0};
struct list free_th_slots = {0};

struct process *new_process_slot() {
        if (free_proc_slots.head) {
                return list_pop_front(&free_proc_slots);
        } else {
                return proc_region++;
        }
}

struct thread *new_thread_slot() {
        if (free_th_slots.head) {
                return list_pop_front(&free_th_slots);
        } else {
                return thread_region++;
        }
}

void free_process_slot(struct process *defunct) {
        list_prepend(&free_proc_slots, defunct);
}

void free_thread_slot(struct thread *defunct) {
        list_prepend(&free_th_slots, defunct);
}

ng_static int process_matches(pid_t wait_arg, struct process *proc) {
        if (wait_arg == 0) {
                return 1;
        } else if (wait_arg > 0) {
                return wait_arg == proc->pid;
        } else if (wait_arg < 0) {
                return -wait_arg == proc->pgid;
        }
        return 0;
}

void threads_init() {
        DEBUG_PRINTF("init_threads()\n");
        
        proc_region = vmm_reserve(512 * 1024 + 102);
        thread_region = vmm_reserve(1 * 1024*1024 + 104);

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

ng_static void set_kernel_stack(void *stack_top) {
        extern uintptr_t *kernel_stack;
        *&kernel_stack = stack_top;
}

ng_static void enqueue_thread(struct thread *th) {
        // Thread 0 is the default and requests to enqueue it should
        // be ignored
        if (th->tid == 0)  return;
        list_append(&runnable_thread_queue, th);
}

ng_static void enqueue_thread_at_front(struct thread *th) {
        // Thread 0 is the default and requests to enqueue it should
        // be ignored
        if (th->tid == 0)  return;
        list_prepend(&runnable_thread_queue, th);
}

// currently in boot.asm
uintptr_t read_ip();

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
        while (true) { 
                struct thread *to = list_pop_front(q);
                if (!to)  return NULL;

                // thread exitted already
                // need to save this memory somewhere to clean it up.
                if (to->state != THREAD_RUNNING)  continue;
                
                // process exitted already
                // need to save this memory somewhere to clean it up.
                if (to->proc->status > 0)  continue;

                return to;
        }
}

#define PROC_RUN_NS 5000

#define SW_TAKE_LOCK 0

void switch_thread(int reason) {
#if SW_TAKE_LOCK
        // trying to take the lock and continuing if we cannot is probably
        // not the thing we want to do in the SW_BLOCK case, but maybe
        // anyone calling SW_BLOCK should be aware that they could be woken
        // up early and re-call switch_thread until they're actually provably
        // ready.  This is something to think about.
        if (!try_acquire_mutex(&process_lock)) {
                // printf("blocked by the process lock\n");
                interrupt_in_ns(1000);
                return;
        }
#endif

        struct thread *to = next_runnable_thread(&runnable_thread_queue);

        if (running_thread->state == THREAD_RUNNING && reason == SW_BLOCK) {
                running_thread->state = THREAD_BLOCKED;
        }

        if (to && running_thread->state == THREAD_RUNNING) {
                enqueue_thread(running_thread);
        }

        if (!to && running_thread->state == THREAD_RUNNING) {
                // this thread ran out of time, but no one else is ready
                // to be run so give it another time slot.

                interrupt_in_ns(PROC_RUN_NS);
#if SW_TAKE_LOCK
                release_mutex(&process_lock);
#endif
                return;
        }

        if (!to) {
                to = &thread_zero;
        }

        DEBUG_PRINTF("[am %i, to %i]\n", running_thread->tid, to->tid);

        struct process *to_proc = to->proc;
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
                // returned after task switch
                interrupt_in_ns(PROC_RUN_NS);
#if SW_TAKE_LOCK
                release_mutex(&process_lock);
#endif
                return;
        }
        running_thread->ip = ip;

        running_process = to_proc;
        running_thread = to;

        interrupt_in_ns(PROC_RUN_NS);
#if SW_TAKE_LOCK
        release_mutex(&process_lock);
#endif

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
                : "%rbx", "%rsp", "%rax",
                  "%rcx", "%r8", "%r9", "%r10", "%r11",
                  "%r12", "%r13", "%r14", "%r15", "memory"
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
        list_append(&proc_zero.children, th);

        release_mutex(&process_lock);

        th->state = THREAD_RUNNING;

        enqueue_thread(th);
}

void return_from_interrupt();

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
        dmgr_insert(&proc->fds, ofd_stdin);
        dmgr_insert(&proc->fds, ofd_stdout);
        dmgr_insert(&proc->fds, ofd_stderr);

        th->tid = tid;
        th->stack = (char *)new_kernel_stack() - 8;
        th->bp = th->stack - sizeof(struct interrupt_frame);
        th->sp = th->bp;
        th->ip = (uintptr_t)return_from_interrupt;
        th->proc = proc;
        // th->flags = THREAD_STRACE;

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

        enqueue_thread_at_front(th);
        switch_thread(SW_BLOCK);
}

noreturn ng_static void do_thread_exit(int exit_status, int thread_state) {
        DEBUG_PRINTF("do_thread_exit(%i, %i)\n", exit_status, thread_state);
        running_thread->state = thread_state;

        // TODO:
        // this may be fragile to context swaps happening during this fucntion
        // I should consider and remediate that

        struct thread *defunct = dmgr_drop(&threads, running_thread->tid);
        free_thread_slot(defunct);

        list_remove(&running_process->threads, running_thread);

        if (running_process->threads.head) {
                // This thread can be removed from the running queue,
                // as it will never run again.
                // printf("still more threads\b");
                switch_thread(SW_BLOCK);
        }

        if (running_process->pid == 1) {
                panic("attempted to kill init!");
        }

        running_process->status = exit_status + 1;
        // This was the last thread - need to wait for a wait on the process.

        struct list_n *node = running_process->parent->threads.head;
        for (; node; node = node->next) {
                struct thread *th = node->v;
                // printf("checking thread %i\n", th->tid);
                if ((th->flags & THREAD_WAIT) == 0) {
                        continue;
                }
                if (process_matches(th->request_status, running_process)) {
                        th->status_resp = running_process;
                        th->state = THREAD_RUNNING;
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

        switch_thread(SW_BLOCK);

        panic("How did we get here?");
}

noreturn struct syscall_ret sys_exit(int exit_status) {
        do_thread_exit(exit_status, THREAD_DONE);
}

noreturn void exit_kthread() {
        do_thread_exit(0, THREAD_DONE);
}

noreturn void kill_running_thread(int exit_status) {
        do_thread_exit(exit_status, THREAD_KILLED_FOR_VIOLATION);
}

struct syscall_ret sys_fork(struct interrupt_frame *r) {
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
#if I686
        // see below
        new_th->stack -= 4;
#endif
        new_th->bp = new_th->stack - sizeof(struct interrupt_frame);
        new_th->sp = new_th->bp;
        new_th->ip = (uintptr_t)return_from_interrupt;
        new_th->proc = new_proc;
        new_th->flags = running_thread->flags;

        struct interrupt_frame *frame = thread_frame(new_th);
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
        new_th->flags = running_thread->flags;

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

        DEBUG_PRINTF("sys_execve(<frame>, \"%s\", <argv>, <envp>)\n", filename);

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
        //
        // sys_waitpid.  At some point in the future this will be
        // updated for compatability with the real wait4.
        //
        RETURN_ERROR(EPERM);
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
        free_process_slot(dmgr_drop(&processes, proc->pid));
}

struct syscall_ret sys_waitpid(pid_t process, int *status, int options) {
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
                if (p->status > 0) {
                        // can clean up now
                        exit_code = p->status - 1;
                        found_pid = p->pid;
                        destroy_child_process(p);

                        *status = exit_code;
                        RETURN_VALUE(found_pid);
                }
        }

        if (!found_candidate) {
                RETURN_ERROR(ECHILD);
        }

        if (options & WNOHANG) {
                RETURN_VALUE(0);
        }

        running_thread->request_status = process;
        running_thread->status_resp = 0;

        running_thread->state = THREAD_BLOCKED;
        running_thread->flags |= THREAD_WAIT;

        while (running_thread->status_resp == 0) {
                switch_thread(SW_BLOCK);
                // *********** rescheduled when a wait() comes in.
                // see do_thread_exit()
        }

        struct process *p = running_thread->status_resp;
        exit_code = p->status - 1;
        found_pid = p->pid;

        destroy_child_process(p);

        running_thread->request_status = 0;
        running_thread->status_resp = NULL;
        running_thread->flags &= ~THREAD_WAIT;

        *status = exit_code;
        RETURN_VALUE(found_pid);
}

struct syscall_ret sys_strace(bool enable) {
        if (enable) {
                running_thread->flags |= THREAD_STRACE;
        } else { 
                running_thread->flags &= ~THREAD_STRACE;
        }
        RETURN_VALUE(enable);
}

void block_thread(struct list *blocked_threads) {
        DEBUG_PRINTF("** block %i\n", running_thread->tid);
        // printf("** block %i\n", running_thread->tid);

        running_thread->state = THREAD_BLOCKED;
        list_append(blocked_threads, running_thread);

        // whoever sets the thread blocking is responsible for bring it back
        switch_thread(SW_BLOCK);
}

ng_static void wake_blocked_thread(void *th_) {
        struct thread *th = th_;
        DEBUG_PRINTF("** wake %i\n", th->tid);

        th->state = THREAD_RUNNING;
        enqueue_thread(th);
}

void wake_blocked_threads(struct list *blocked_threads) {
        if (blocked_threads->head) {
                list_foreach(blocked_threads, wake_blocked_thread);
                list_free(blocked_threads);
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

ng_static void _kill_this_pgid(void *process) {
        struct process *proc = process;

        if (proc == running_process) {
                return;
        }

        if (proc->pgid == running_process->pgid) {
                // threads will not schedule if their process has an exit status
                //
                // How does this interact with SIGCHLD?
                // *shrugs*
                proc->status = 0xFFFF;
        }
}

struct syscall_ret sys_exit_group(int exit_status) {
        dmgr_foreach(&processes, _kill_this_pgid);
        running_process->status = exit_status + 1;
        do_thread_exit(exit_status, THREAD_DONE);
}

void _print_thread(void *thread) {
        struct thread *th = thread;

        char *wait = th->flags & THREAD_WAIT ? "W" : " ";
        char *status = th->state == THREAD_RUNNING ? "r" : "B";
        
        printf("  t: %i %s%s\n", th->tid, wait, status);
}

void _print_process(void *process) {
        struct process *proc = process;

        if (proc->status <= 0) {
                printf("pid %i: %s\n", proc->pid, proc->comm);
        } else {
                printf("pid %i: %s (defunct: %i (%x))\n", proc->pid, proc->comm,
                                proc->status, proc->status);
        }

        list_foreach(&proc->threads, _print_thread);
}

struct syscall_ret sys_top(int show_threads) {
        if (!show_threads)
                dmgr_foreach(&processes, _print_process);
        RETURN_VALUE(0);
}

