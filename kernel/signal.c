#include <assert.h>
#include <basic.h>
#include <errno.h>
#include <ng/memmap.h>
#include <ng/signal.h>
#include <ng/syscall.h>
#include <ng/syscall_consts.h>
#include <ng/thread.h>

#define SIGSTACK_LEN 2048

static_assert(NG_SIGRETURN < 0xFF); // sigreturn must fit in one byte

// clang-format off
const unsigned char signal_handler_return[] = {
    // mov rdi, rax
    0x48, 0x89, 0xc7,
    // mov rax, (signal return code)
    0x48, 0xc7, 0xc0, NG_SIGRETURN, 0, 0, 0,
    // int 0x80
    0xCD, 0x80,
};
// clang-format on

// If this grows it needs to change in bootstrap_usermode
static_assert(sizeof(signal_handler_return) < 0x10);

sysret sys_sigaction(int sig, sighandler_t handler, int flags) {
    if (sig < 0 || sig > 32) return -EINVAL;

    // Flags is intended for things like specifying that the signal
    // handler is interested in an additional parameter with more
    // information about the signal. See siginfo_t on Linux.
    if (flags) return -ETODO;

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

    if (old) *old = old_mask;
    return 0;
}

noreturn sysret sys_sigreturn(int code) {
    struct thread *th = running_thread;

    set_kernel_stack(th->kstack);
    th->flags &= ~TF_IN_SIGNAL;

    th->state = th->nonsig_state;

    if (th->state == TS_RUNNING) {
        longjmp(th->kernel_ctx, 2);
    } else {
        struct thread *next = thread_sched();
        thread_switch_nosave(next);
    }
}

int signal_send_th(struct thread *th, int signal) {
    sigaddset(&th->sig_pending, signal);
    thread_enqueue(th);

    return 0;
}

int signal_send(pid_t pid, int signal) {
    // TODO: negative pid pgrp things
    if (pid < 0) return -ETODO;

    struct thread *th = thread_by_id(pid);
    if (!th) return -ESRCH;

    return signal_send_th(th, signal);
}

int signal_send_pgid(pid_t pgid, int signal) {
    list_for_each(struct thread, th, &all_threads, all_threads) {
        struct process *p = th->proc;
        if (th->tid != p->pid) continue;
        if (pgid != p->pgid) continue;
        signal_send_th(th, signal);
    }
    return 0;
}

sysret sys_kill(pid_t pid, int sig) {
    return signal_send(pid, sig);
}


bool signal_is_actionable(struct thread *th, int signal) {
    if (sigismember(&th->sig_mask, signal)) return false;
    return sigismember(&th->sig_pending, signal);
}

int handle_pending_signals() {
    struct thread *th = running_thread;

    for (int signal = 0; signal < 32; signal++) {
        if (!signal_is_actionable(th, signal)) continue;

        sigdelset(&th->sig_pending, signal);
        handle_signal(signal, th->sighandlers[signal]);
    }

    return 0;
}

void signal_self(int signal) {
    handle_signal(signal, running_thread->sighandlers[signal]);
}

void handle_signal(int signal, sighandler_t handler) {
    if (signal == SIGKILL) { kill_process(running_process, signal + 256); }

    // the tracer can change what signal is delivered to the traced thread.
    signal = trace_signal_delivery(signal, handler);
    if (!signal) return;

    if (signal == SIGSTOP) {
        running_thread->flags |= TF_STOPPED;
        return;
    }
    if (signal == SIGCONT) {
        running_thread->flags &= ~TF_STOPPED;
        return;
    }
    if (handler == SIG_IGN) { return; }
    if (handler == SIG_DFL) {
        switch (signal) {
            case SIGCHLD:
            case SIGURG:
            case SIGWINCH:
                return;
            default:
                kill_process(running_process, signal + 256);
        }
    }
    do_signal_call(signal, handler);
}

static char static_signal_stack[SIGSTACK_LEN];
static char *sigstack = static_signal_stack + SIGSTACK_LEN;

void do_signal_call(int sig, sighandler_t handler) {
    struct thread *th = running_thread;
    th->nonsig_state = th->state;
    th->state = TS_RUNNING;
    th->flags |= TF_IN_SIGNAL;

    uintptr_t old_sp = th->user_ctx->user_sp;

    uintptr_t new_sp = round_down(old_sp - 128, 16);

    uintptr_t *pnew_sp = (uintptr_t *)new_sp;
    pnew_sp[0] = SIGRETURN_THUNK; // rbp + 8
    pnew_sp[1] = 0;               // rbp

    set_kernel_stack(sigstack);

    jmp_to_userspace((uintptr_t)handler, new_sp, sig);

    assert(0);
}
