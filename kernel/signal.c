
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

sysret sys_sigaction(int signum, sighandler_t handler, int flags) {
        return error(ETODO);
}

sysret sys_sigreturn(int code) {
        return error(ETODO);
}

sysret sys_kill(pid_t pid, int sig) {
        return error(ETODO);
}

