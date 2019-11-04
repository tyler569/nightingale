
#include <basic.h>
#include <ng/signal.h>
#include <ng/syscall.h>
#include <ng/syscall_consts.h>
#include <ng/thread.h>

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
        if (sig < 0 || sig > 15)  return error(EINVAL);

        // Flags is intended for things like specifying that the signal
        // handler is interested in an additional parameter with more
        // information about the signal. See siginfo_t on Linux.
        if (flags)  return error(ETODO);

        running_process->sigactions[sig] = handler;
        return value(0);
}

sysret sys_sigreturn(int code) {
        return error(ETODO);
}

#define SIGNAL_PGROUP   1
#define SIGNAL_PROCESS  2
#define SIGNAL_CHILDREN 3

int send_signal(int flags, struct process *p, int sig) {
        if (sig < 0 || sig >= 16) {
                return EINVAL;
        }
        
        p->signals_pending |= (1 << sig);
        return 0;
}

sysret sys_kill(pid_t pid, int sig) {
        /* TODO
        if (pid > 0) {
                send_signal(SIGNAL_PROCESS, pid, sig);
        } else if (pid == 0) {
                pid = running_process->pid;
                send_signal(SIGNAL_CHILDREN, pid, sig);
        } else if (pid < 0) {
                return error(ETODO);
                send_signal(SIGNAL_PGROUP, -pid, sig);
        }
        */

        if (pid <= 0)  return error(ETODO);

        struct process *p = process_by_id(pid);
        if (!p)  return error(EINVAL);
        
        int s = send_signal(SIGNAL_PROCESS, p, sig);
        if (s)  return error(s);

        wake_process_thread(p);
        return value(0);
}

