
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
        .magic = PROC_MAGIC,
        .comm = "<nightingale>",
        .parent = NULL,
};

extern char boot_kernel_stack; // boot.asm

struct thread thread_zero = {
        .tid = 0,
        .magic = THREAD_MAGIC,
        .kstack = &boot_kernel_stack,
        .state = THREAD_RUNNING,
        .flags = THREAD_IS_KTHREAD,
};

struct thread *thread_idle = &thread_zero;

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
        assert(!(defunct->flags & THREAD_QUEUED));
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

        proc_zero.vm = vm_kernel;

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
        assert(!(defunct->flags & THREAD_QUEUED));
        DEBUG_PRINTF("freeable(%i)\n", defunct->tid);
        list_append(&freeable_thread_queue, defunct, freeable);
        thread_enqueue(finalizer);
}

bool enqueue_checks(struct thread *th) {
        if (th->tid == 0)  return false;
        if (th->trace_state == TRACE_STOPPED)  return false;
        if (th->flags & THREAD_QUEUED)  return false;
        assert(th->proc->pid > -1);
        assert_thread_not_runnable(th);
        th->flags |= THREAD_QUEUED;
        return true;
}

void thread_enqueue(struct thread *th) {
        disable_irqs();
        if (enqueue_checks(th))
                list_append(&runnable_thread_queue, th, runnable);
        enable_irqs();
}

void thread_enqueue_at_front(struct thread *th) {
        disable_irqs();
        if (enqueue_checks(th))
                list_prepend(&runnable_thread_queue, th, runnable);
        enable_irqs();
}

void wake_thread(struct thread *th) {
        if (th->state != THREAD_RUNNING) {
                th->state = THREAD_RUNNING;
                thread_enqueue(th);
        }
}

void wake_blocked_thread(struct thread *th) {
        if (th->state != THREAD_RUNNING) {
                th->state = THREAD_RUNNING;

                // enqueue thread that waited at front for responsiveness
                thread_enqueue_at_front(th);
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

#define thread_save_bpsp(th) do { \
        asm volatile("mov %%rsp, %0" : "=r"((th)->sp)); \
        asm volatile("mov %%rbp, %0" : "=r"((th)->bp)); \
} while (0);

#elif I686

#define thread_save_bpsp(th) do { \
        asm volatile("mov %%esp, %0" : "=r"((th)->sp)); \
        asm volatile("mov %%ebp, %0" : "=r"((th)->bp)); \
} while(0);

#endif

struct thread *next_runnable_thread() {
        struct thread *rt;
        rt = list_pop_front(struct thread, &runnable_thread_queue, runnable);
        if (rt) {
                rt->flags &= ~THREAD_QUEUED;
        }
        return rt;
}

/*
 * Choose the next thread to run.
 *
 * This procedure disables interrupts and expectc you to re-enable them
 * when you're done doing whatever you need to with this information.
 *
 * It does dequeue the thread from the runnable queue, so consider that
 * if you don't actually plan on running it.
 */
struct thread *thread_sched(void) {
        disable_irqs();

        struct thread *to;
        to = next_runnable_thread();
        return to;
}

void thread_set_running(struct thread *th) {
        running_thread->flags &= ~THREAD_ONCPU;

        running_process = th->proc;
        running_thread = th;
        running_thread->flags |= THREAD_ONCPU;
}

// formerly SW_BLOCK
void thread_block(void) {
        struct thread *to = thread_sched();

        if (!to) {
                to = thread_idle;
        }
        assert(to->magic == THREAD_MAGIC);

        // It is expected that the caller will wake the thread when it is
        // needed later.
        
        thread_switch(to, running_thread);
}

// formerly SW_YIELD
void thread_yield(void) {
        struct thread *to = thread_sched();

        if (!to) {
                enable_irqs();
                return;
        }
        assert(to->magic == THREAD_MAGIC);

        thread_enqueue(running_thread);

        thread_switch(to, running_thread);
}

// formerly SW_TIMEOUT
void thread_timeout(void) {
        struct thread *to = thread_sched();

        if (!to) {
                enable_irqs();
                return;
        }
        assert(to->magic == THREAD_MAGIC);

        thread_enqueue(running_thread);

        thread_switch(to, running_thread);
}

// formerly SW_DONE
void thread_done(void) {
        struct thread *to = thread_sched();

        if (!to) {
                to = thread_idle;
        }
        assert(to->magic == THREAD_MAGIC);

        thread_switch(to, running_thread);
        assert(0);
}


bool thread_needs_fpu(struct thread *th) {
        return th->proc->pid != 0;
}

void thread_switch(struct thread *new, struct thread *old) {
        set_kernel_stack(new->kstack);

        if (thread_needs_fpu(old))
                fxsave(&old->fpctx);
        if (thread_needs_fpu(new))
                fxrstor(&new->fpctx);

        // TODO bottle this up
        if (new->proc->vm_root != old->proc->vm_root && !(new->flags & THREAD_IS_KTHREAD))
                set_vm_root(new->proc->vm_root);

        thread_set_running(new);

        thread_save_bpsp(old);
        uintptr_t ip = read_ip();
        old->ip = ip;

        if (ip == 0x99) {
                enable_irqs();
                handle_pending_signals();
                return;
        }

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
                : "b"(new->ip), "c"(new->sp), "d"(new->bp)
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

                "sti \n\t"
                "jmp *%%ebx"
                :
                : "r"(new->ip), "r"(new->sp), "r"(new->bp)
                : "%ebx", "%eax"
        );
#endif
}

void process_procfile(struct open_file *ofd) {
        struct file *node = ofd->node;
        struct process *p = node->memory;
        ofd->buffer = malloc(4096);
        int x = 0;
        /*
        x += sprintf(ofd->buffer + x, "Process %i\n", p->pid);
        x += sprintf(ofd->buffer + x, "  name: \"%s\"\n", p->comm);
        x += sprintf(ofd->buffer + x, "  parent: %i (\"%s\")\n",
                        p->parent->pid, p->parent->comm);
        x += sprintf(ofd->buffer + x, "  vm_root: %#zx\n", p->vm->pgtable_root);
        x += sprintf(ofd->buffer + x, "  pgid: %i\n", p->pgid);
        x += sprintf(ofd->buffer + x, "  mmap_base: %#zx\n", p->mmap_base);
        */
        x += sprintf(ofd->buffer + x, "%i '%s' %i %i %i\n",
                p->pid, p->comm, p->parent->pid, p->pgid, p->exit_status);

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
                th->tid, th->proc->pid, th->state, th->flags, 
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
        virt_addr_t new_stack = vm_alloc(4 * PAGE_SIZE);
        void *stack_top = (void *)(new_stack + 4 * PAGE_SIZE);

        return stack_top;
}

struct thread *new_thread() {
        struct thread *th = new_thread_slot();
        int new_tid = dmgr_insert(&threads, th);
        memset(th, 0, sizeof(struct thread));
        th->magic = THREAD_MAGIC;

        th->kstack = (char *)new_kernel_stack();

        th->tid = new_tid;
        th->bp = th->kstack - 16;
        th->sp = th->bp - sizeof(struct interrupt_frame);

        struct interrupt_frame *frame = thread_frame(th);
        memset(frame, 0, sizeof(struct interrupt_frame));

        create_thread_procfile(th);

        frame_set(frame, FLAGS, INTERRUPT_ENABLE);

        th->state = THREAD_RUNNING;

        return th;
}

struct thread *new_kthread(uintptr_t entrypoint) {
        DEBUG_PRINTF("new_kernel_thread(%#lx)\n", entrypoint);

        struct thread *th = new_thread();

        th->ip = entrypoint;
        th->proc = &proc_zero;
        th->flags = THREAD_IS_KTHREAD,
        list_append(&proc_zero.threads, th, process_threads);

        thread_enqueue(th);
        return th;
}

struct thread *process_thread(struct process *p) {
        return list_head_entry(struct thread, &p->threads, process_threads);
}

struct process *new_process(struct thread *th) {
        struct process *proc = new_process_slot();
        memset(proc, 0, sizeof(struct process));
        proc->magic = PROC_MAGIC;

        list_init(&proc->children);
        list_init(&proc->threads);
        dmgr_init(&proc->fds);

        proc->pid = th->tid;
        proc->parent = running_process;
        _list_append(&running_process->children, &proc->siblings);
        _list_append(&proc->threads, &th->process_threads);

        create_process_procfile(proc);

        return proc;
}

struct process *new_user_process(uintptr_t entrypoint) {
        DEBUG_PRINTF("new_user_process(%#lx)\n", entrypoint);
        struct thread *th = new_thread();
        struct process *proc = new_process(th);

        strcpy(proc->comm, "<init>");
        proc->mmap_base = USER_MMAP_BASE;

        th->ip = (uintptr_t)return_from_interrupt;
        th->proc = proc;
        // th->flags = THREAD_STRACE;

        th->cwd = fs_resolve_relative_path(fs_root_node, "/bin");
        th->thread_flags = THREAD_STRACE;

        struct interrupt_frame *frame = thread_frame(th);

        frame->ds = 0x18 | 3;
        frame_set(frame, IP, entrypoint);
        frame_set(frame, SP, USER_STACK - 16);
        frame_set(frame, BP, USER_STACK - 16);

        vm_user_alloc(map, USER_STACK - 0x10000, USER_STACK, VM_COW);
        vm_user_alloc(map, USER_ARGV, USER_ARGV + 0x20000, VM_COW);

        // TODO: x86ism
        frame->cs = 0x10 | 3;
        frame->ss = 0x18 | 3;
        frame_set(frame, FLAGS, INTERRUPT_ENABLE);

        proc->vm = vm_user_new();
        th->state = THREAD_RUNNING;

        return proc;
}

struct process *bootstrap_usermode(const char *init_filename) {
        vm_user_alloc(usermap, SIGRETURN_THUNK, SIGRETURN_THUNK + 0x1000, VM_COW);

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
        thread_enqueue(child_thread);

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
                thread_enqueue(th_1);
        }

        return 0;
}

void finalizer_kthread(void) {
        while (true) {
                struct thread *th;
                th = list_pop_front(struct thread, &freeable_thread_queue, freeable);

                if (!th) {
                        thread_block();
                } else {
                        // printf("finalize_thread(%i)\n", th->tid);
                        free_thread_slot(th);
                }
        }
}

void wake_waiting_parent_thread(void) {
        struct process *parent = running_process->parent;
        struct thread *parent_th;
        list_foreach(&parent->threads, parent_th, process_threads) {
                if ((parent_th->flags & THREAD_WAIT) == 0) {
                        continue;
                }
                if (process_matches(parent_th->wait_request, running_process)) {
                        parent_th->wait_result = running_thread;
                        signal_send_th(parent_th, SIGCHLD);
                }
        }
}

void thread_cleanup(void) {
        list_remove(&running_thread->wait_node);
        list_remove(&running_thread->process_threads);
        list_remove(&running_thread->runnable);

        if (running_thread->wait_event) {
                drop_timer_event(running_thread->wait_event);
        }

        dmgr_drop(&threads, running_thread->tid);

        if (running_thread->procfile) {
                destroy_file(running_thread->procfile);
                running_thread->procfile = NULL;
        }

        running_thread->magic = 0;
}

noreturn void do_thread_exit(int exit_status, enum thread_state state) {
        DEBUG_PRINTF("do_thread_exit(%i, %i)\n", exit_status, state);
        struct thread *dead = running_thread;
        dead->state = state;

        disable_irqs();

        thread_cleanup();

        if (!list_empty(&running_process->threads)) {
                enable_irqs();
                thread_done();

                assert(0); // This thread can never run again
        }

        do_process_exit(exit_status);
}

noreturn void do_process_exit(int exit_status) {
        struct process *dead_proc = running_process;
        struct process *parent = dead_proc->parent;

        if (dead_proc->pid == 1) {
                panic("attempted to kill init!");
        }

        if (dead_proc->pid == 0) {
                enable_irqs();
                thread_done();
        }

        dead_proc->exit_status = exit_status + 1;

        wake_waiting_parent_thread();

        enable_irqs();
        thread_done();

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

        struct thread *new_th = new_thread();
        struct process *new_proc = new_process(new_th);

        strncpy(new_proc->comm, running_process->comm, COMM_SIZE);
        new_proc->pgid = running_process->pgid;
        new_proc->uid = running_process->uid;
        new_proc->gid = running_process->gid;
        new_proc->mmap_base = running_process->mmap_base;

        // copy files to child
        deep_copy_fds(&new_proc->fds, &running_process->fds);

        new_th->user_sp = running_thread->user_sp;

        new_th->ip = (uintptr_t)return_from_interrupt;
        new_th->proc = new_proc;
        new_th->flags = running_thread->flags;
        new_th->cwd = running_thread->cwd;
        new_th->flags &= ~THREAD_STRACE;

        struct interrupt_frame *frame = thread_frame(new_th);
        memcpy(frame, r, sizeof(interrupt_frame));
        frame_set(frame, RET_VAL, 0);
        frame_set(frame, RET_ERR, 0);

        new_proc->vm = vm_user_fork(running_process->vm);
        new_th->state = THREAD_RUNNING;

        thread_enqueue(new_th);
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
        new_th->flags = running_thread->flags;
        new_th->cwd = running_thread->cwd;

        struct interrupt_frame *frame = thread_frame(new_th);
        memcpy(frame, r, sizeof(interrupt_frame));
        frame_set(frame, RET_VAL, 0);
        frame_set(frame, RET_ERR, 0);

        frame_set(frame, SP, (uintptr_t)new_stack);
        frame_set(frame, BP, (uintptr_t)new_stack);
        frame_set(frame, IP, (uintptr_t)fn);

        new_th->state = THREAD_RUNNING;

        thread_enqueue(new_th);

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

        strncpy(running_process->comm, node->filename, COMM_SIZE);

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
        vm_user_exec(running_process->vm);

        // redo arguments for vm world ^
        //
        // char *argument_data = (void *)USER_ENVP;
        // char **user_argv = (void *)USER_ARGV;
        // size_t argc = 0;
        // while (*argv) {
        //         // printf("[DEBUG] : argument is %p:\"%s\"\n", *argv, *argv);
        //         user_argv[argc] = argument_data;
        //         argument_data = strcpy(argument_data, *argv) + 1;
        //         argc += 1;
        //         argv += 1;
        // }
        // user_argv[argc] = 0;

        assert(0); // TODO mappings in fork
        elf_load(elf, NULL);

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
                printf("%p\n", child);
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
        dmgr_free(&proc->fds);
        destroy_file(proc->procfile);
        list_remove(&proc->siblings);
        vm_user_exit(proc->vm);
        dmgr_drop(&processes, proc->pid);
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

        running_thread->state = THREAD_BLOCKED;
        running_thread->flags |= THREAD_WAIT;

        while (running_thread->wait_result == NULL) {
                thread_block();
                // rescheduled when a wait() comes in
                // see wake_waiting_parent_thread();
        }

        struct thread *wait_thread = running_thread->wait_result;
        struct process *p = wait_thread->proc;
        exit_code = p->exit_status - 1;
        found_pid = p->pid;

        destroy_child_process(p);

        running_thread->wait_request = 0;
        running_thread->wait_result = NULL;
        running_thread->flags &= ~THREAD_WAIT;

        *status = exit_code;
        return found_pid;
}

sysret sys_strace(bool enable) {
        if (enable) {
                running_thread->flags |= THREAD_STRACE;
        } else { 
                running_thread->flags &= ~THREAD_STRACE;
        }
        return enable;
}

void block_thread(list *blocked_threads) {
        DEBUG_PRINTF("** block %i\n", running_thread->tid);

        running_thread->state = THREAD_BLOCKED;
        list_append(blocked_threads, running_thread, wait_node);

        // whoever sets the thread blocking is responsible for bring it back
        thread_block();
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
                thread_yield();
        }
}

sysret sys_yield(void) {
        thread_yield();
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
                th->state = THREAD_KILLED;
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
                thread_yield();
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
        char *wait = th->flags & THREAD_WAIT ? "W" : " ";

        char *status;
        switch (th->state) {
        case THREAD_RUNNING: status = "r"; break;
        case THREAD_KILLED:  status = "Z"; break;
        default:             status = "B"; break;
        }

        printf("  t: %i %s%s%s\n", th->tid, wait, status,
               th->flags & THREAD_ONCPU ? "*" : "");
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
        
        th->state = THREAD_RUNNING;
        // th->flags |= THREAD_WOKEN;

        thread_enqueue_at_front(th);
        thread_yield();
}

void wake_thread_from_sleep(void *thread) {
        struct thread *th = thread;
        //list_remove(&th->wait_node);
        th->wait_event = NULL;

        wake_thread(th);
}

sysret sys_sleepms(int ms) {
        running_thread->state = THREAD_SLEEP;
        struct timer_event *te = insert_timer_event(
                milliseconds(ms), wake_thread_from_sleep, running_thread);
        running_thread->wait_event = te;
        
        thread_block();
        return 0;
}

void thread_timer(void *_) {
        insert_timer_event(THREAD_TIME, thread_timer, NULL);
        thread_timeout();
}

