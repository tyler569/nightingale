#include <basic.h>
#include <assert.h>
#include <ng/cpu.h>
#include <ng/irq.h>
#include <ng/pmm.h>
#include <ng/spalloc.h>
#include <ng/tests.h>
#include <ng/thread.h>
#include <ng/timer.h>
#include <ng/x86/pic.h>

void run_sync_tests(void);

void test_kernel_thread(void *arg) {
    printf("Hello World from a kernel thread\n");
    const char *message = arg;
    printf("The message is '%s'!\n", message);
    kthread_exit();
}

void lots_of_threads(void *message) {
    printf("%s", message);
    kthread_exit();
}

noreturn void test_sleepy_thread(void *_) {
    while (true) {
        printf("sleepy thread");
        sleep_thread(seconds(1));
    }
}

void print_key(interrupt_frame *frame, void *_x) {
    char scancode = inb(0x80);
    printf("keyboard interrupt: %c\n", scancode);
}

sysret sys_syscall_test(char *buffer) {
    strcpy(buffer, "pizza");
    pm_summary();
    return 0;
}

void run_spalloc_test() {
    // validate spalloc working
    struct testing {
        int a, b, c, d, e, f, g, h;
    };
    struct spalloc foobar;
    sp_init(&foobar, struct testing);

    struct testing *first = sp_alloc(&foobar);
    assert(first == foobar.region);
    first->a = 10;

    struct testing *second = sp_alloc(&foobar);
    assert(second == sp_at(&foobar, 1));
    second->a = 11;

    assert(first->a == 10);

    first->g = 1;
    sp_free(&foobar, first);
    assert(first->g != 1); // poison
    assert(second->a == 11);

    struct testing *re_first = sp_alloc(&foobar);
    assert(re_first == first);

    assert(foobar.capacity == 0x1000);
    assert(foobar.count == 2);
}

void run_all_tests() {
    run_sync_tests();
    run_spalloc_test();
    kthread_create(test_kernel_thread, "get a cat");
    // kthread_create(test_sleepy_thread, NULL);

    for (int i = 0; i < 50; i++) {
        kthread_create(lots_of_threads, "");
        kthread_create(lots_of_threads, "");
    }

    pic_irq_unmask(IRQ_KEYBOARD);
    irq_install(IRQ_KEYBOARD, print_key, NULL);
}
