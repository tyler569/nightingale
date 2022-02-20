#include <assert.h>
#include <setjmp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define DEBUG 0

enum uthread_state {
    UTHREAD_RUNNING = 1,
    UTHREAD_DONE,
};

struct uthread {
    volatile enum uthread_state uthread_state;
    char *stack;
    int tid;
    jmp_buf execution_state;
};

extern struct uthread *running_uthread;

#define STACK_SIZE (1024 * 1024)

int uthread_create(struct uthread *thread, void (*func)(int), int arg);
void uthread_init(void);
struct uthread *uthread_sched(void);
void uthread_yield(void);
void uthread_exit(void);
void uthread_join(struct uthread *th);

#define MAX_UTHREADS 1024

struct uthread *active_threads[MAX_UTHREADS] = {0};
struct uthread *running_uthread = NULL;
struct uthread thread_one = {0};

void uthread_init() {
    active_threads[0] = &thread_one;
    running_uthread = &thread_one;
    thread_one.uthread_state = UTHREAD_RUNNING;
    setjmp(thread_one.execution_state);
}

int uthread_create(struct uthread *thread, void (*func)(int), int arg) {
    int use_slot = MAX_UTHREADS;
    for (int i = 0; i < MAX_UTHREADS; i++) {
        if (active_threads[i] == NULL) {
            use_slot = i;
            break;
        }
    }
    if (use_slot == MAX_UTHREADS) {
        // cannot create thread, at maximum
        return 1;
    }

    thread->uthread_state = UTHREAD_RUNNING;
    thread->stack = malloc(STACK_SIZE);
    thread->tid = use_slot;
    active_threads[use_slot] = thread;

    struct uthread *return_to = running_uthread;

    if (setjmp(running_uthread->execution_state)) {
        running_uthread = return_to;
        return 0;
    }

    running_uthread = thread;
    asm volatile ("mov %0, %%rsp\n\t" : : "g" (thread->stack + STACK_SIZE));
    func(arg);
    assert(0);
}

struct uthread *uthread_sched() {
    static int pos = 4;

    struct uthread *candidate;
    while (true) {
        pos += 1;
        if (pos > 7)
            pos = 0;
        candidate = active_threads[pos];
        if (candidate && candidate->uthread_state == UTHREAD_RUNNING)
            break;
    }
    return candidate;
}

void uthread_yield() {
    if (DEBUG)
        printf("yield %d\n", running_uthread->tid);

    struct uthread *volatile return_to = NULL;

    if (setjmp(running_uthread->execution_state)) {
        if (DEBUG)
            printf("enter %d\n", running_uthread->tid);
        assert(return_to);
        running_uthread = return_to;
        return;
    }
    struct uthread *next_uthread = uthread_sched();
    if (next_uthread == running_uthread)
        return;
    return_to = running_uthread;
    longjmp(next_uthread->execution_state, 1);
}

void uthread_exit() {
    if (DEBUG)
        printf("exit %d\n", running_uthread->tid);
    running_uthread->uthread_state = UTHREAD_DONE;
    uthread_yield();
}

void uthread_join(struct uthread *th) {
    if (DEBUG)
        printf("[%d]: join %d\n", running_uthread->tid, th->tid);
    while (th->uthread_state == UTHREAD_RUNNING) {
        uthread_yield();
    }
}

void ab(int ab) {
    for (int i = 0; i < 1000; i++) {
        printf("%c", ab);
        uthread_yield();
    }
    printf("\n");
    uthread_exit();
}

int main() {
    uthread_init();

    struct uthread a, b;

    printf("making a\n");
    uthread_create(&a, ab, 'a');
    printf("making b\n");
    uthread_create(&b, ab, 'b');
    printf("make b\n");

    printf("joining on a\n");
    uthread_join(&a);
    printf("joining on b\n");
    uthread_join(&b);

    printf("done, goodbye!\n");
    return EXIT_SUCCESS;
}
