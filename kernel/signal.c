
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
        if (sig < 0 || sig > 32)  return -EINVAL;

        // Flags is intended for things like specifying that the signal
        // handler is interested in an additional parameter with more
        // information about the signal. See siginfo_t on Linux.
        if (flags)  return -ETODO;

        running_thread->sighandlers[sig] = handler;
        return 0;
}

sysret sys_sigprocmask(int op, const sigset_t *new, sigset_t *old) {
        struct thread *th = running_thread;
        sigset_t old_mask = th->sig_mask;

        switch (op) {
        case SIG_BLOCK:
                th->sig_mask |= *new;
                break;
        case SIG_UNBLOCK:
                th->sig_mask &= ~(*new);
                break;
        case SIG_SETMASK:
                th->sig_mask = *new;
                break;
        default:
                return -EINVAL;
        }

        if (old)  *old = old_mask;
        return 0;
}

noreturn sysret sys_sigreturn(int code) {
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

        disable_irqs();
        noreturn void thread_context_load(struct thread *th);
        thread_context_load(th);
}

sysret sys_kill(pid_t pid, int sig) {
        return signal_send(pid, sig);
}

int signal_send(pid_t pid, int signal) {
        // TODO: negative pid pgrp things
        if (pid < 0)  return -ETODO;
        
        struct thread *th = thread_by_id(pid);
        if (!th)  return -ESRCH;

        sigaddset(&th->sig_pending, signal);
        enqueue_thread(th);

        return 0;
}

bool signal_is_actionable(struct thread *th, int signal) {
        if (sigismember(&th->sig_mask, signal)) return false;
        return sigismember(&th->sig_pending, signal);
}

int handle_pending_signals() {
        struct thread *th = running_thread;
        int handled = 0;

        for (int signal=0; signal<32; signal++) {
                if (signal_is_actionable(th, signal)) {
                        handle_signal(signal, th->sighandlers[signal]);
                        sigdelset(&th->sig_pending, signal);
                        handled += 1;
                }
        }

        if (handled > 1)
                printf("handle_panding_signals handled %i\n", handled);
        return handled;
}

void signal_self(int signal) {
        handle_signal(signal, running_thread->sighandlers[signal]);
}

void handle_signal(int signal, sighandler_t handler) {
        if (signal == SIGKILL) {
                do_thread_exit(0, THREAD_KILLED);
        }

        int disp = trace_signal_delivery(signal, handler);
        if (disp == TRACE_SIGNAL_SUPPRESS) {
                return;
        }

        if (signal == SIGSTOP) {
                // definitely something correct to do
        }
        if (handler == SIG_IGN) {
                return;
        }
        if (handler == SIG_DFL) {
                switch (signal) {
                case SIGCHLD:
                case SIGURG:
                case SIGWINCH:
                        return;
                default:
                        do_thread_exit(0, THREAD_KILLED);
                }
        }
        do_signal_call(signal, handler);
}

char static_signal_stack[SIGNAL_KERNEL_STACK];

void do_signal_call(int sig, sighandler_t handler) {
        struct thread *th = running_thread;
        struct interrupt_frame *r;
        r = (struct interrupt_frame *)
                ((uintptr_t)th->sp - 256 - sizeof(struct interrupt_frame));

        th->signal_context.ip = th->ip;
        th->signal_context.sp = th->sp;
        th->signal_context.bp = th->bp;
        th->signal_context.stack = th->stack;
        th->signal_context.thread_state = th->thread_state;

        /*
        //char *stack = malloc(SIGNAL_KERNEL_STACK); // TODO ?
        char *stack = static_signal_stack;
        th->stack = stack + SIGNAL_KERNEL_STACK;
        set_kernel_stack(th->stack);
        th->sp = th->stack - 16;
        th->bp = th->stack - 16;
        */

        // EVIL EVIL x86ism from thread.c
        r->cs = 0x10 | 3;
        r->ds = 0x18 | 3;
        r->ss = 0x18 | 3;

#if X86_64
        uintptr_t old_sp = th->user_sp;
        
        uintptr_t new_sp = old_sp - 256;

        unsigned long *sp = (unsigned long *)new_sp;
        sp[-1] = 0;
        sp[0] = SIGRETURN_THUNK;
        sp[1] = 0;
        r->user_rsp = new_sp;
        r->rbp = new_sp;
        r->rflags = 0x200; // IF
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
        uintptr_t old_sp = th->user_sp;
        
        uintptr_t new_sp = old_sp - 256;

        unsigned long *sp = (unsigned long *)new_sp;
        sp[-1] = 0;
        sp[0] = SIGRETURN_THUNK;
        sp[1] = 0;
        r->user_esp = new_sp;
        r->ebp = new_sp;
        r->eflags = 0x200; // IF
        r->edi = sig;
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

