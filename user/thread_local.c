#include <basic.h>
#include <nightingale.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int __ng_settls(void *base);
struct tcb;
struct tcb *alloc_tcb(void);
_Thread_local int tls;
_Noreturn void exit_thread(int code);
char stack[8192];

int use_tls(void)
{
    int tid = gettid();
    printf("tid is %i, tls is ", tid);
    tls = 10 + tid;
    printf("%i\n", tls);
    return 0;
}

int thread(void *arg)
{
    alloc_tcb();
    use_tls();
    exit_thread(0);
}

int main()
{
    alloc_tcb();
    clone(thread, stack + 8192, 0, NULL);
    use_tls();

    sleepms(30);
}

struct tcb {
    struct tcb *self;
    void *base;
};

struct tcb *alloc_tcb(void)
{
    void *base = calloc(1, 1024);
    struct tcb *tcb = PTR_ADD(
        base, round_down(1024 - sizeof(struct tcb), _Alignof(struct tcb)));
    tcb->self = tcb;
    tcb->base = base;
    __ng_settls(tcb);
    return tcb;
}
