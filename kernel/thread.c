
// #define DEBUG
#include <basic.h>
#include <ng/debug.h>
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
#include <ng/procfile.h>
#include <ng/signal.h>
#include <nc/errno.h>
#include <nc/sys/wait.h>
#include <linker/elf.h>
#include <stddef.h>
#include <stdint.h>

extern uintptr_t boot_pt_root;

list runnable_thread_queue = {0};
list freeable_thread_queue = {0};
struct thread *finalizer = NULL;

void finalizer_kthread(void);
void thread_timer(void *);

#define THREAD_TIME milliseconds(5)

// kmutex process_lock = KMUTEX_INIT;
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

struct process *new_process_slot() {
        return malloc(sizeof(struct process));
}

struct thread *new_thread_slot() {
        return malloc(sizeof(struct thread));
}

void free_process_slot(struct process *defunct) {
        free(defunct);
}

void free_thread_slot(struct thread *defunct) {
        assert(!(defunct->thread_flags & THREAD_QUEUED));
        free(defunct);
}

int process_matches(pid_t wait_arg, struct process *proc) {
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

struct thread *thread_by_id(pid_t tid) {
        struct thread *th = dmgr_get(&threads, tid);
        return th;
}

struct process *process_by_id(pid_t pid) {
        struct thread *th = thread_by_id(pid);
        return th->proc;
}

void threads_init() {
        DEBUG_PRINTF("init_threads()\n");

        dmgr_init(&threads);
        list_init(&runnable_thread_queue);
        list_init(&freeable_thread_queue);
        list_init(&proc_zero.threads);
        list_init(&proc_zero.children);

        thread_zero.proc = &proc_zero;
        thread_zero.cwd = fs_root_node;
        
        dmgr_insert(&threads, &thread_zero);
        dmgr_insert(&threads, (void *)1); // save 1 for init

        list_append(&proc_zero.threads, &thread_zero, process_threads);
        printf("threads: process structures initialized\n");

        finalizer = new_kthread((uintptr_t)finalizer_kthread);
        printf("threads: finalizer thread running\n");

        insert_timer_event(milliseconds(10), thread_timer, NULL);
        printf("threads: thread_timer started\n");
}

struct interrupt_frame *thread_frame(struct thread *th) {
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

void assert_thread_not_runnable(struct thread *th) {
        struct thread *q_thread;

        list_foreach(&runnable_thread_queue, q_thread, runnable) {
                assert(q_thread != th);
        }
}

void make_freeable(struct thread *defunct) {
        assert(!(defunct->thread_flags & THREAD_QUEUED));
        DEBUG_PRINTF("freeable(%i)\n", defunct->tid);
        list_append(&freeable_thread_queue, defunct, freeable);
        enqueue_thread(finalizer);
}

/*
void drop_thread(struct thread *th) {
        if (th->tid == 0)  return;
        list_remove(&runnable_thread_queue, th);
}
*/

bool enqueue_checks(struct thread *th) {
        if (th->tid == 0)  return false;
        if (th->trace_state == TRACE_STOPPED)  return false;
        if (th->thread_flags & THREAD_QUEUED)  return false;
        assert(th->proc->pid > -1);
        assert_thread_not_runnable(th);
        th->thread_flags |= THREAD_QUEUED;
        return true;
}

void enqueue_thread(struct thread *th) {
        disable_irqs();
        if (enqueue_checks(th))
                list_append(&runnable_thread_queue, th, runnable);
        enable_irqs();
}

void enqueue_thread_at_front(struct thread *th) {
        disable_irqs();
        if (enqueue_checks(th))
                list_prepend(&runnable_thread_queue, th, runnable);
        enable_irqs();
}

void wake_thread(struct thread *th) {
        if (th->thread_state != THREAD_RUNNING) {
                th->thread_state = THREAD_RUNNING;
                enqueue_thread(th);
        }
}

void wake_blocked_thread(struct thread *th) {
        if (th->thread_state != THREAD_RUNNING) {
                th->thread_state = THREAD_RUNNING;

                // enqueue thread that waited at front for responsiveness
                enqueue_thread_at_front(th);
        }
}

// currently in boot.asm
__attribute__((returns_twice))
extern uintptr_t read_ip(void);

// portability!
void fxsave(fp_ctx *fpctx) {
        // printf("called fxsave with %p\n", fpctx);
#if X86_64
        asm volatile("fxsaveq %0" ::"m"(*fpctx));
#elif I686
        asm volatile("fxsave %0" ::"m"(*fpctx));
#endif
}

void fxrstor(fp_ctx *fpctx) {
        // printf("called fxrstor with %p\n", fpctx);
#if X86_64
        asm volatile("fxrstorq %0" : "=m"(*fpctx));
#elif I686
        asm volatile("fxrstor %0" : "=m"(*fpctx));
#endif
}

#if X86_64

#define save_running_bpsp() do { \
        asm volatile("mov %%rsp, %0" : "=r"(running_thread->sp)); \
        asm volatile("mov %%rbp, %0" : "=r"(running_thread->bp)); \
} while (0);

#elif I686

#define save_running_bpsp() do { \
        asm volatile("mov %%esp, %0" : "=r"(running_thread->sp)); \
        asm volatile("mov %%ebp, %0" : "=r"(running_thread->bp)); \
} while(0);

#endif

struct thread *next_runnable_thread() {
        struct thread *rt;
        rt = list_pop_front(struct thread, &runnable_thread_queue, runnable);
        if (rt) {
                rt->thread_flags &= ~THREAD_QUEUED;
        }
        return rt;
}

void switch_thread(enum switch_reason reason) { // "schedule"
        disable_irqs();

        struct thread *to;

        switch (reason) {
        case SW_BLOCK: {
                to = next_runnable_thread();
                if (!to)  to = &thread_zero;
                break;
        }
        case SW_YIELD: // FALLTHROUGH
        case SW_TIMEOUT: {
                to = next_runnable_thread();
                if (!to) {
                        enable_irqs();
                        return;
                }
                if (running_thread->thread_state == THREAD_RUNNING)
                        enqueue_thread(running_thread);
                break;
        }
        case SW_DONE: {
                to = next_runnable_thread();
                if (!to)  to = &thread_zero;
                make_freeable(running_thread);
                break;
        }
        default: {
                panic("invalid switch reason");
        }
        }

        switch_thread_to(to);
}

// must be called with interrupts disabled ?
void switch_thread_to(struct thread *to) {
        if (to == running_thread) {
                enable_irqs();
                return;
        }

        DEBUG_PRINTF("[am %i, to %i]\n", running_thread->tid, to->tid);

        set_kernel_stack(to->stack);
        if (running_process->pid != 0) {
                fxsave(&running_thread->fpctx);
        }
        if (to->proc->pid != 0) {
                set_vm_root(to->proc->vm_root);
                fxrstor(&to->fpctx);
        }

        save_running_bpsp();
        uintptr_t ip = read_ip();

        if (ip == 0x99) {
                enable_irqs();

                handle_pending_signals();
                return;
        }

        running_thread->ip = ip;
        running_thread->thread_flags &= ~THREAD_ONCPU;

        running_process = to->proc;
        running_thread = to;
        running_thread->thread_flags |= THREAD_ONCPU;

#if X86_64
        asm volatile(
                "mov %0, %%rbx\n\t"
                "mov %1, %%rsp\n\t"
                "mov %2, %%rbp\n\t"

                // This makes read_ip return 0x99 when we switch back to it
                "mov $0x99, %%rax\n\t"

                "sti \n\t"
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

noreturn void thread_context_load(struct thread *th) {
#if X86_64
        asm volatile (
                "mov %0, %%rbx\n\t"
                "mov %1, %%rsp\n\t"
                "mov %2, %%rbp\n\t"

                // This makes read_ip return 0x99 when we switch back to it
                "mov $0x99, %%rax\n\t"

                "sti \n\t"
                "jmp *%%rbx"
                :
                : "b"(th->ip), "c"(th->sp), "d"(th->bp)
                : "%rax", "%rsi", "%rdi", "%r8", "%r9", "%r10",
                  "%r11", "%r12", "%r13", "%r14", "%r15", "memory"
        );
#elif I686
        asm volatile (
                "mov %0, %%ebx\n\t"
                "mov %1, %%esp\n\t"
                "mov %2, %%ebp\n\t"

                // This makes read_ip return 0x99 when we switch back to it
                "mov $0x99, %%eax\n\t"

                "jmp *%%ebx"
                :
                : "r"(th->ip), "r"(th->sp), "r"(th->bp)
                : "%ebx", "%eax"
        );
#endif
        assert(0);
}

void process_procfile(struct open_file *ofd) {
        struct file *node = ofd->node;
        struct process *p = node->memory;
        ofd->buffer = malloc(4096);
        int x = 0;
        x += sprintf(ofd->buffer + x, "Process %i\n", p->pid);
        x += sprintf(ofd->buffer + x, "  name: \"%s\"\n", p->comm);
        x += sprintf(ofd->buffer + x, "  parent: %i (\"%s\")\n",
                        p->parent->pid, p->parent->comm);
        x += sprintf(ofd->buffer + x, "  vm_root: %#zx\n", p->vm_root);
        x += sprintf(ofd->buffer + x, "  pgid: %i\n", p->pgid);
        x += sprintf(ofd->buffer + x, "  mmap_base: %#zx\n", p->mmap_base);

        ofd->length = x;
}

void create_process_procfile(struct process *p) {
        char name[32];
        sprintf(name, "%i", p->pid);
        struct file *procfile = make_procfile(name, process_procfile, p);
        p->procfile = procfile;
}

void thread_procfile(struct open_file *ofd) {
        struct file *node = ofd->node;
        struct thread *th = node->memory;
        ofd->buffer = malloc(4096);
        int x = 0;

        x += sprintf(ofd->buffer + x, "%i %i %i %i %i %i %x %x %li %li %li\n",
                th->tid, th->proc->pid, th->thread_state, th->thread_flags, 
                th->wait_request, th->trace_state, th->sig_pending, th->sig_mask,
                th->n_scheduled, th->time_ran, th->last_scheduled);

        ofd->length = x;
}

void create_thread_procfile(struct thread *th) {
        char name[32];
        sprintf(name, "th%i", th->tid);
        struct file *procfile = make_procfile(name, thread_procfile, th);
        th->procfile = procfile;
}

void *new_kernel_stack() {
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

struct thread *new_thread() {
        struct thread *th = new_thread_slot();
        int new_tid = dmgr_insert(&threads, th);
        memset(th, 0, sizeof(struct thread));

        th->stack = (char *)new_kernel_stack();

        th->tid = new_tid;
        th->bp = th->stack;
        th->sp = th->bp - sizeof(struct interrupt_frame) - 16;

        struct interrupt_frame *frame = thread_frame(th);
        memset(frame, 0, sizeof(struct interrupt_frame));

        create_thread_procfile(th);

        frame_set(frame, FLAGS, INTERRUPT_ENABLE);

        th->thread_state = THREAD_RUNNING;

        return th;
}

struct thread *new_kthread(uintptr_t entrypoint) {
        DEBUG_PRINTF("new_kernel_thread(%#lx)\n", entrypoint);

        struct thread *th = new_thread();

        th->ip = entrypoint;
        th->proc = &proc_zero;
        list_append(&proc_zero.threads, th, process_threads);

        enqueue_thread(th);
        return th;
}

struct thread *process_thread(struct process *p) {
        return list_head_entry(struct thread, &p->threads, process_threads);
}

struct process *new_user_process(uintptr_t entrypoint) {
        DEBUG_PRINTF("new_user_process(%#lx)\n", entrypoint);
        struct process *proc = new_process_slot();
        struct thread *th = new_thread();

        memset(proc, 0, sizeof(struct process));

        list_init(&proc->children);
        list_init(&proc->threads);

        proc->pid = -1;
        proc->comm = "<init>";
        proc->parent = 0;
        proc->mmap_base = USER_MMAP_BASE;
        proc->parent = running_process;
        list_append(&running_process->children, proc, siblings);
        list_append(&proc->threads, th, process_threads);

        proc->pid = th->tid;
        dmgr_init(&proc->fds);

        /*
        This is now done by init to support multiple login shells

        dmgr_insert(&proc->fds, ofd_stdin);
        dmgr_insert(&proc->fds, ofd_stdout);
        dmgr_insert(&proc->fds, ofd_stderr);
        */

        th->ip = (uintptr_t)return_from_interrupt;
        th->proc = proc;
        // th->thread_flags = THREAD_STRACE;
        th->cwd = fs_resolve_relative_path(fs_root_node, "/bin");

        struct interrupt_frame *frame = thread_frame(th);

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

        create_process_procfile(proc);
        proc->vm_root = vmm_fork();
        th->thread_state = THREAD_RUNNING;

        return proc;
}

struct process *bootstrap_usermode(const char *init_filename) {
        vmm_create_unbacked_range(SIGRETURN_THUNK, 0x1000,
                        PAGE_USERMODE | PAGE_WRITEABLE); // make read-only
        memcpy((void *)SIGRETURN_THUNK, signal_handler_return, 0x10);

        struct file *cwd = running_thread->cwd;
        if (!cwd)  cwd = fs_root_node;

        struct file *init = fs_resolve_relative_path(cwd, init_filename);
        
        assert(init);
        assert(init->filetype == FT_BUFFER);
        Elf *program = init->memory;
        assert(elf_verify(program));

        elf_load(program);
        // printf("Starting ring 3 thread at %#zx\n\n", program->e_entry);

        dmgr_drop(&threads, 1);
        struct process *child = new_user_process(program->e_entry);
        struct thread *child_thread = process_thread(child);
        enqueue_thread(child_thread);

        return child;
}

void deep_copy_fds(struct dmgr *child_fds, struct dmgr *parent_fds) {
        struct open_file *pfd, *cfd;
        for (int i=0; i<parent_fds->cap; i++) {
                if ((pfd = dmgr_get(parent_fds, i)) == 0) {
                        continue;
                }
                // printf("copy fd %i (\"%s\")\n", i, pfd->node->filename);
                cfd = malloc(sizeof(struct open_file));
                memcpy(cfd, pfd, sizeof(struct open_file));
                pfd->node->refcnt++;
                dmgr_set(child_fds, i, cfd);
        }
}

sysret sys_create(const char *executable) {
        struct process *child = bootstrap_usermode(executable);
        return child->pid;
}

sysret sys_procstate(pid_t destination, enum procstate flags) {
        struct process *d_p = process_by_id(destination);
        struct process *p = running_process;

        if (flags & PS_COPYFDS) {
                deep_copy_fds(&d_p->fds, &p->fds);
        }

        if (flags & PS_SETRUN) {
                struct thread *th_1 = list_head_entry(struct thread, &d_p->threads, process_threads);
                enqueue_thread(th_1);
        }

        return 0;
}

void finalizer_kthread(void) {
        while (true) {
                struct thread *th;
                th = list_pop_front(struct thread, &freeable_thread_queue, freeable);

                if (!th) {
                        switch_thread(SW_BLOCK);
                } else {
                        // printf("finalize_thread(%i)\n", th->tid);
                        free_thread_slot(th);
                }
        }
}

noreturn void do_thread_exit(int exit_status, enum thread_state state) {
        DEBUG_PRINTF("do_thread_exit(%i, %i)\n", exit_status, state);
        struct thread *dead = running_thread;
        dead->thread_state = state;

        // TODO:
        // this may be fragile to context swaps happening during this fucntion
        // I should consider and remediate that
        
        list_remove(&running_thread->wait_node);
        list_remove(&running_thread->process_threads);

        if (running_thread->wait_event) {
                drop_timer_event(running_thread->wait_event);
        }
        dmgr_drop(&threads, dead->tid);

        assert_thread_not_runnable(dead);
        if (dead->procfile) {
                destroy_file(dead->procfile);
                dead->procfile = NULL;
        }
        if (!list_empty(&running_process->threads)) {
                // This thread can be removed from the running queue,
                // as it will never run again.
                //
                // TODO: this is a race condition:
                // the last two threads can slip through here, thus leaving
                // a process with no threads and waits blocking forever.
                switch_thread(SW_DONE);
        }

        struct process *dead_proc = running_process;
        struct process *parent = dead_proc->parent;

        if (dead_proc->pid == 1) {
                panic("attempted to kill init!");
        }

        if (dead_proc->pid == 0) {
                switch_thread(SW_DONE);
        }

        dead_proc->exit_status = exit_status + 1;
        // This was the last thread - need to wait for a wait on the process.

        struct thread *parent_th;
        list_foreach(&parent->threads, parent_th, process_threads) {
                if ((parent_th->thread_flags & THREAD_WAIT) == 0) {
                        continue;
                }
                if (process_matches(parent_th->wait_request, running_process)) {
                        parent_th->wait_result = running_thread;
                        wake_thread(parent_th);
                }
        }
        // If we got here, no one is listening currently.
        // someone may call waitpid(2) later, so leave this around and if
        // it doesn't get cleaned up it becomes a zombie.
        //
        // This thread can be removed from the running queue, as it will
        // never run again.

        switch_thread(SW_DONE);

        assert(0); // This thread can never run again
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
        struct thread *new_th = new_thread();

        pid_t new_pid = new_th->tid;

        //memcpy(new_proc, running_process, sizeof(struct process));
        memset(new_proc, 0, sizeof(struct process));

        list_init(&new_proc->children);
        list_init(&new_proc->threads);

        new_proc->pid = new_pid;
        new_proc->parent = running_process;
        new_proc->comm = malloc(strlen(running_process->comm));
        strcpy(new_proc->comm, running_process->comm);
        new_proc->pgid = running_process->pgid;
        new_proc->uid = running_process->uid;
        new_proc->gid = running_process->gid;
        new_proc->mmap_base = running_process->mmap_base;

        // copy files to child
        dmgr_init(&new_proc->fds);
        deep_copy_fds(&new_proc->fds, &running_process->fds);

        list_append(&running_process->children, new_proc, siblings);
        list_append(&new_proc->threads, new_th, process_threads);

        new_th->user_sp = running_thread->user_sp;

        new_th->ip = (uintptr_t)return_from_interrupt;
        new_th->proc = new_proc;
        new_th->thread_flags = running_thread->thread_flags;
        new_th->cwd = running_thread->cwd;
        new_th->thread_flags &= ~THREAD_STRACE;

        struct interrupt_frame *frame = thread_frame(new_th);
        memcpy(frame, r, sizeof(interrupt_frame));
        frame_set(frame, RET_VAL, 0);
        frame_set(frame, RET_ERR, 0);

        new_proc->vm_root = vmm_fork();
        new_th->thread_state = THREAD_RUNNING;

        create_process_procfile(new_proc);
        enqueue_thread(new_th);

        sysret ret = new_proc->pid;

        return ret;
}

sysret sys_clone0(struct interrupt_frame *r, int (*fn)(void *), 
                  void *new_stack, void *arg, int flags) {
        DEBUG_PRINTF("sys_clone0(%#lx, %p, %p, %p, %i)\n",
                        r, fn, new_stack, arg, flags);

        if (running_process->pid == 0) {
                panic("Cannot clone() the kernel\n");
        }

        struct thread *new_th = new_thread();

        list_append(&running_process->threads, new_th, process_threads);

        new_th->ip = (uintptr_t)return_from_interrupt;
        new_th->proc = running_process;
        new_th->thread_flags = running_thread->thread_flags;
        new_th->cwd = running_thread->cwd;

        struct interrupt_frame *frame = thread_frame(new_th);
        memcpy(frame, r, sizeof(interrupt_frame));
        frame_set(frame, RET_VAL, 0);
        frame_set(frame, RET_ERR, 0);

        frame_set(frame, SP, (uintptr_t)new_stack);
        frame_set(frame, BP, (uintptr_t)new_stack);
        frame_set(frame, IP, (uintptr_t)fn);

        new_th->thread_state = THREAD_RUNNING;

        enqueue_thread(new_th);

        return new_th->tid;
}

sysret sys_getpid() {
        return running_process->pid;
}

sysret sys_gettid() {
        return running_thread->tid;
}

extern struct tar_header *initfs;

sysret do_execve(struct file *node, struct interrupt_frame *frame,
                 char **argv, char **envp) {

        if (running_process->pid == 0) {
                panic("cannot execve() the kernel\n");
        }

        // if (!(node->perms & USR_EXEC))  return -ENOEXEC;

        char *new_comm = malloc(strlen(node->filename));
        strcpy(new_comm, node->filename);
        running_process->comm = new_comm;

        if (!(node->filetype == FT_BUFFER))  return -ENOEXEC;
        char *file = node->memory;
        if (!file)  return -ENOENT;

        if (file[0] == '#' && file[1] == '!') {
                // EVIL CHEATS
                struct file *sh = fs_resolve_relative_path(NULL, "/bin/sh");
                file = sh->memory;
                struct open_file *ofd = zmalloc(sizeof(struct open_file));
                ofd->node = node;
                ofd->flags = USR_READ;
                dmgr_set(&running_process->fds, 0, ofd);
        }

        Elf *elf = (Elf *)file;
        if (!elf_verify(elf))  return -ENOEXEC;

        // pretty sure I shouldn't use the environment area for argv...
        char *argument_data = (void *)USER_ENVP;
        char **user_argv = (void *)USER_ARGV;
        size_t argc = 0;
        while (*argv) {
                user_argv[argc] = argument_data;
                argument_data = strcpy(argument_data, *argv) + 1;
                argc += 1;
                argv += 1;
        }
        user_argv[argc] = 0;

        // INVALIDATES POINTERS TO USERSPACE
        elf_load(elf);

        running_process->mmap_base = USER_MMAP_BASE;

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

        frame_set(frame, ARGC, argc);
        frame_set(frame, ARGV, (uintptr_t)user_argv);

        return 0;
}

sysret sys_execve(struct interrupt_frame *frame, char *filename,
                              char **argv, char **envp) {
        DEBUG_PRINTF("sys_execve(<frame>, \"%s\", <argv>, <envp>)\n", filename);

        struct file *file = fs_resolve_relative_path(
                        running_thread->cwd, filename);
        if (!file)  return -ENOENT;

        return do_execve(file, frame, argv, envp);
}

sysret sys_execveat(struct interrupt_frame *frame,
                        int dir_fd, char *filename,
                        char **argv, char **envp) {
        struct open_file *ofd = dmgr_get(&running_process->fds, dir_fd);
        if (!ofd)  return -EBADF;
        struct file *node = ofd->node;
        if (node->filetype != FT_DIRECTORY)  return -EBADF;

        struct file *file = fs_resolve_relative_path(node, filename);
        return do_execve(file, frame, argv, envp);
}

sysret sys_wait4(pid_t process) {
        return -ETODO;
}

void close_open_fd(void *fd) {
        struct open_file *ofd = fd;
        // printf("closing '%s'\n", ofd->node->filename);
        do_close_open_file(ofd);
}

void destroy_child_process(struct process *proc) {
        // printf("destroying pid %i\n", proc->pid);
        disable_irqs();
        assert(proc != running_process);
        assert(proc->exit_status);

        struct process *init = process_by_id(1);
        struct process *child;
        list_foreach(&proc->children, child, siblings) {
                child->parent = init;
        }
        list_concat(&init->children, &proc->children);

        struct thread *th;
        list_foreach(&proc->threads, th, process_threads) {
                assert(th->wait_node.next == NULL);
                assert(th->wait_node.prev == NULL);
        }
        dmgr_foreach(&proc->fds, close_open_fd);
        assert(list_length(&proc->threads) == 0);
        // list_free(&proc->children);
        // list_free(&proc->threads); // should be empty
        dmgr_free(&proc->fds);
        destroy_file(proc->procfile);
        free(proc->comm);
        list_remove(&proc->siblings);
        vmm_destroy_tree(proc->vm_root);
        free_process_slot(proc);
        enable_irqs();
}

sysret sys_waitpid(pid_t process, int *status, enum wait_options options) {
        int exit_code;
        int found_pid;
        int found_candidate = 0;

        DEBUG_PRINTF("waitpid(%i, xx, xx)\n", process);

        struct process *child, *tmp;
        list_foreach_safe(&running_process->children, child, tmp, siblings) {
                if (process_matches(process, child)) {
                        found_candidate = 1;
                } else {
                        continue;
                }
                if (child->exit_status > 0) {
                        // can clean up now
                        exit_code = child->exit_status - 1;
                        found_pid = child->pid;
                        destroy_child_process(child);

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

        struct thread *wait_thread = running_thread->wait_result;
        struct process *p = wait_thread->proc;
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

void block_thread(list *blocked_threads) {
        DEBUG_PRINTF("** block %i\n", running_thread->tid);

        running_thread->thread_state = THREAD_BLOCKED;
        list_append(blocked_threads, running_thread, wait_node);

        // whoever sets the thread blocking is responsible for bring it back
        switch_thread(SW_BLOCK);
}

void wake_blocked_threads(list *blocked_threads) {
        int threads_awakened = 0;

        struct thread *th, *tmp;

        list_foreach_safe(blocked_threads, th, tmp, wait_node) {
                list_remove(&th->wait_node);
                wake_blocked_thread(th);
                threads_awakened++;
        }

        if (threads_awakened > 0) {
                switch_thread(SW_YIELD);
        }
}

sysret sys_yield(void) {
        switch_thread(SW_YIELD);
        return 0;
}

sysret sys_setpgid(int pid, int pgid) {
        if (pid != running_process->pid) {
                return -EPERM;
        }
        running_process->pgid = pgid;
        return 0;
}

sysret sys_exit_group(int exit_status) {
        kill_process_group(running_process->pgid);
        running_process->exit_status = exit_status + 1;
        do_thread_exit(exit_status, THREAD_DONE);
}

void kill_thread(struct thread *th) {
        if (th == running_thread) {
                // This is likely a bad idea if we're in kill_process, since
                // we should clean up all of the process's threads before we
                // exit -- think about this more.
                do_thread_exit(1, THREAD_KILLED);
        } else {
                th->thread_state = THREAD_KILLED;
        }
}

void kill_process(struct process *p) {
        struct thread *th, *tmp;

        list_foreach_safe(&p->threads, th, tmp, process_threads) {
                kill_thread(th);
        }

        if (p == running_process) {
                // This is likely a bad idea if we're in kill_process_group,
                // since we should clean up all of the process's threads
                // before we exit -- think about this more.
                //
                // Same problem in kill_thread
                switch_thread(SW_YIELD);
        }
}

void kill_pid(pid_t pid) {
        struct process *p = process_by_id(pid);
        if (p)  kill_process(p);
}

static void _kill_pg_by_id(void *process, long pgid) {
        struct process *proc = process;
        if (proc->pgid == pgid) {
                struct thread *th, *tmp;
                list_foreach_safe(&proc->threads, th, tmp, process_threads) {
                        kill_thread(th);
                }
        }
}

void kill_process_group(pid_t pgid) {
}


void _print_thread(struct thread *th) {
        char *wait = th->thread_flags & THREAD_WAIT ? "W" : " ";

        char *status;
        switch (th->thread_state) {
        case THREAD_RUNNING: status = "r"; break;
        case THREAD_KILLED:  status = "Z"; break;
        default:             status = "B"; break;
        }

        printf("  t: %i %s%s%s\n", th->tid, wait, status,
               th->thread_flags & THREAD_ONCPU ? "*" : "");
}

void _print_process(void *p) {
        struct process *proc = p;
        struct thread *th;

        if (proc->exit_status <= 0) {
                printf("pid %i: %s\n", proc->pid, proc->comm);
        } else {
                printf("pid %i: %s (defunct: %i)\n",
                        proc->pid, proc->comm, proc->exit_status);
        }

        list_foreach(&proc->threads, th, process_threads) {
                _print_thread(th);
        }
}

sysret sys_top(int show_threads) {
        return -EINVAL;
}

void wake_process_thread(struct process *p) {
        struct thread *th = process_thread(p);
        assert(th);
        
        th->thread_state = THREAD_RUNNING;
        // th->thread_flags |= THREAD_WOKEN;

        enqueue_thread_at_front(th);
        switch_thread(SW_YIELD);
}

void wake_thread_from_sleep(void *thread) {
        struct thread *th = thread;
        //list_remove(&th->wait_node);
        th->wait_event = NULL;

        wake_thread(th);
}

sysret sys_sleepms(int ms) {
        running_thread->thread_state = THREAD_SLEEP;
        struct timer_event *te = insert_timer_event(
                milliseconds(ms), wake_thread_from_sleep, running_thread);
        running_thread->wait_event = te;
        
        switch_thread(SW_BLOCK);
        return 0;
}

void thread_timer(void *_) {
        insert_timer_event(THREAD_TIME, thread_timer, NULL);
        switch_thread(SW_TIMEOUT);
}

