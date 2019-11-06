
#include <basic.h>
#include <ng/signal.h>
#include <ng/syscall.h>
#include <ng/syscall_consts.h>
#include <ng/thread.h>
#include <ng/memmap.h>
#include <nc/assert.h>
#include <nc/errno.h>

#define SIGNAL_KERNEL_STACK 2048

static_assert(NG_SIGRETURN < 0xFF, "sigreturn must fit in one byte");

const unsigned char signal_handler_return[] = {
#if X86_64
        // mov rdi, rax
        0x48, 0x89, 0xc7,
        // mov al, (signal return code)
        0x48, 0xc7, 0xc0, NG_SIGRETURN, 0, 0, 0,
        // int 0x80
        0xCD, 0x80,
#elif I686
        // mov rdi, rax
        0x89, 0xc7,
        // mov al, (signal return code)
        0xb8, NG_SIGRETURN, 0, 0, 0,
        // int 0x80
        0xCD, 0x80,
#endif
};

static_assert(sizeof(signal_handler_return) < 0x10, "change in bootstrap_usermode");

sysret sys_sigaction(int sig, sighandler_t handler, int flags) {
        if (sig < 0 || sig > 15)  return -EINVAL;

        // Flags is intended for things like specifying that the signal
        // handler is interested in an additional parameter with more
        // information about the signal. See siginfo_t on Linux.
        if (flags)  return -ETODO;

        running_process->sigactions[sig] = handler;
        return 0;
}

sysret sys_sigreturn(int code) {
        struct thread *th = running_thread;

        // free(th->stack); -- static for now - change?
        
        th->ip = th->signal_context.ip;
        th->sp = th->signal_context.sp;
        th->bp = th->signal_context.bp;
        th->stack = th->signal_context.stack;
        th->thread_state = th->signal_context.thread_state;

        th->thread_flags &= ~THREAD_IN_SIGNAL;

        if (th->thread_flags & THREAD_AWOKEN) {
                th->thread_flags &= ~THREAD_AWOKEN;
                th->thread_state = THREAD_RUNNING;
        }

        switch_thread(SW_REQUEUE);

        assert(0); // noreturn;
}

void send_signal_to_process(struct process *p, int sig) {
        if (sig < p->signal_pending || p->signal_pending == 0) {
                p->signal_pending = sig;
        }
        wake_process_thread(p);
}

int send_signal(pid_t pid, int sig) {
        /* TODO
        if (pid > 0) {
                send_signal(SIGNAL_PROCESS, pid, sig);
        } else if (pid == 0) {
                -- TODO
                proc_foreach_child_recurse(send_signal_to_process, sig);
        } else if (pid < 0) {
                -- TODO
                proc_foreach_in_prgoup(-pid, sig);
        }
        */

        if (sig < 0 || sig >= 16) {
                return EINVAL;
        }
        
        struct process *p = process_by_id(pid);
        if (!p)  return EINVAL;

        send_signal_to_process(p, sig);

        return 0;
}

sysret sys_kill(pid_t pid, int sig) {
        if (pid <= 0)  return -ETODO;

        int s = send_signal(pid, sig);
        if (s)  return -s;

        return 0;
}

void handle_pending_signal() {
        int sig = running_process->signal_pending;
        struct thread *th = running_thread;
        struct process *p = th->proc;

        if (sig == 0) {
                // nothing to do here
                return;
        }

        if (sig < 0 || sig > 15) {
                th->thread_state = THREAD_KILLED;
                return;
        }

        if (sig < 3 && p->sigactions[sig] == SIG_DFL) {
                kill_process(running_process);
        }
        if (p->sigactions[sig] == SIG_IGN) {
                return;
        }

        do_signal_call(sig, p->sigactions[sig]);
}

void send_immediate_signal_to_self(int sig) {
        struct process *p = running_thread->proc;

        if (sig < 3 && p->sigactions[sig] == SIG_DFL) {
                kill_process(running_process);
        }
        if (p->sigactions[sig] == SIG_IGN) {
                return;
        }

        do_signal_call(sig, p->sigactions[sig]);
}

char static_signal_stack[SIGNAL_KERNEL_STACK];

void do_signal_call(int sig, sighandler_t handler) {
        struct thread *th = running_thread;
        struct process *p = th->proc;
        struct interrupt_frame *r;
        r = (struct interrupt_frame *)((uintptr_t)th->sp - 256 - sizeof(struct interrupt_frame));

        p->signal_pending = 0;
        
        th->signal_context.ip = th->ip;
        th->signal_context.sp = th->sp;
        th->signal_context.bp = th->bp;
        th->signal_context.stack = th->stack;
        th->signal_context.thread_state = th->thread_state;

        //char *stack = malloc(SIGNAL_KERNEL_STACK); // TODO ?
        char *stack = static_signal_stack;
        th->stack = stack + SIGNAL_KERNEL_STACK;
        set_kernel_stack(th->stack);
        th->sp = th->stack - 16;
        th->bp = th->stack - 16;

        /* 
         * NOTE NOTE NOTE NOTE NOTE
         *
         * This function is live tiwddling the stack above it.
         * Do not call functions that take a lot of stack space
         * (e.g. printf) after this memcpy!
         *
         * You will corrupt your new frame!!
         *
         * You will be sad!!!
         */
        //memcpy(r, th->user_frame, sizeof(struct interrupt_frame));

        // EVIL EVIL x86ism from thread.c
        r->cs = 0x10 | 3;
        r->ds = 0x18 | 3;
        r->ss = 0x18 | 3;

#if X86_64
        uintptr_t old_sp = th->user_frame->user_rsp;
        
        uintptr_t new_sp = old_sp - 256;

        unsigned long *sp = (unsigned long *)new_sp;
        *sp = SIGRETURN_THUNK;
        *(sp + 1) = 0;
        r->user_rsp = new_sp;
        r->rbp = new_sp;
        r->rdi = sig;
        r->rip = (uintptr_t)handler;
        asm volatile (
                "mov %0, %%rsp \n\t"
                "mov %0, %%rbp \n\t"
                "jmp *%1 \n\t"
                :
                : "rm"(r), "b"(return_from_interrupt)
        );
#elif I686
        uintptr_t old_sp = r->user_esp;
        
        uintptr_t new_sp = old_sp - 256;

        unsigned long *sp = (unsigned long *)new_sp;
        sp[-1] = 0;
        sp[0] = SIGRETURN_THUNK;
        sp[1] = 0;
        r->user_esp = new_sp;
        r->ebp = new_sp;
        r->eip = (uintptr_t)handler;

        char *stack_at_jump = (void *)r;
        stack_at_jump -= 4;
        asm volatile (
                "mov %0, %%esp \n\t"
                "mov %0, %%ebp \n\t"
                "jmp *%1 \n\t"
                :
                : "rm"(stack_at_jump), "b"(return_from_interrupt)
        );
#endif

        assert(0);
}

