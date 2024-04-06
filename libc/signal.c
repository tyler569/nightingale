#include <ng/common.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

int sigemptyset(sigset_t *set) {
	*set = 0;
	return 0;
}

int sigfillset(sigset_t *set) {
	*set = -1;
	return 0;
}

int sigaddset(sigset_t *set, int signum) {
	*set |= (1u << signum);
	return 0;
}

int sigdelset(sigset_t *set, int signum) {
	*set &= ~(1u << signum);
	return 0;
}

int sigismember(const sigset_t *set, int signum) {
	return (*set & (1u << signum)) != 0;
}

#ifndef __kernel__

void __ng_sigreturn(void);
sighandler_t __ng_sigaction(int, sighandler_t, int);

static sighandler_t __signal_handlers[32] = {};

static void __handle_signal(int signal) {
	if (__signal_handlers[signal]) {
		__signal_handlers[signal](signal);
	}
	__ng_sigreturn();
}

static bool __is_function(sighandler_t handler) {
	return (uintptr_t)handler >= 0x1000;
}

sighandler_t signal(int signum, sighandler_t handler) {
	sighandler_t original = 0, kernel_original = 0;

	if (handler == NULL || __is_function(handler)) {
		original = __signal_handlers[signum];
		__signal_handlers[signum] = handler;
		kernel_original = __ng_sigaction(signum, __handle_signal, 0);
	} else {
		kernel_original = __ng_sigaction(signum, handler, 0);
	}

	if (kernel_original == __handle_signal)
		return original;
	else
		return kernel_original;
}

sighandler_t sigaction(int signum, sighandler_t handler, int flags) {
	return signal(signum, handler);
}

int raise(int signal) { return kill(getpid(), signal); }

#endif
