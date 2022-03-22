#include <stdio.h>
#include <nightingale.h>
#include <unistd.h>

int __ng_settls(void *base);

char stack[8192];

int tls_storage[2] = { 0 };

void *tls_a_ptr = &tls_storage[1];
void *tls_b_ptr = &tls_storage[2];

_Thread_local int tls;

_Noreturn void exit_thread(int code);

int use_tls(void *arg)
{
    int v = *(int *)arg;
    if (v)
        __ng_settls(&tls_b_ptr);

    int tid = gettid();
    printf("tid is %i, tls is ", tid);
    tls = 10 + tid;
    printf("%i\n", tls);

    if (v)
        exit_thread(0);
    return 0;
}

int true_ = 1;
int false_ = 0;

int main()
{
    __ng_settls(&tls_a_ptr);
    clone(use_tls, stack + 8192, 0, &true_);
    use_tls(&false_);

    sleepms(30);

    printf("after: [");
    for (int i = 0; i < 2; i++) {
        printf("%i", tls_storage[i]);
        if (i < 1)
            printf(", ");
    }
    printf("]\n");
}
