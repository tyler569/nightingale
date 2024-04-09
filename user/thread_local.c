#include <nightingale.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int __ng_settls(void *base);
struct tcb;
struct tcb *alloc_tcb(void);
thread_local int tls;
[[noreturn]] void exit_thread(int code);
char stack[8192];

int use_tls(void) {
	int tid = gettid();
	printf("tid is %i, tls is ", tid);
	tls = 10 + tid;
	printf("%i\n", tls);
	return 0;
}

int thread(void *arg) {
	alloc_tcb();
	use_tls();
	exit_thread(0);
}

int main() {
	alloc_tcb();
	clone(thread, stack + 8192, 0, nullptr);
	use_tls();

	sleepms(30);
}

struct tcb {
	struct tcb *self;
	void *base;
};

struct tcb *alloc_tcb(void) {
	void *base = calloc(1, 1024);
	struct tcb *tcb = PTR_ADD(
		base, ROUND_DOWN(1024 - sizeof(struct tcb), alignof(struct tcb)));
	tcb->self = tcb;
	tcb->base = base;
	__ng_settls(tcb);
	return tcb;
}
