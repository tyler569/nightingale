#include <basic.h>
#include <assert.h>
#include <ng/cpu.h>
#include <ng/irq.h>
#include <ng/pmm.h>
#include <ng/tests.h>
#include <ng/thread.h>
#include <ng/timer.h>
#include <x86/pic.h>

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
    return 0;
}

void run_all_tests() {
    run_sync_tests();
    kthread_create(test_kernel_thread, "get a cat");
    // kthread_create(test_sleepy_thread, NULL);

    for (int i = 0; i < 50; i++) {
        kthread_create(lots_of_threads, "");
        kthread_create(lots_of_threads, "");
    }

    pic_irq_unmask(IRQ_KEYBOARD);
    irq_install(IRQ_KEYBOARD, print_key, NULL);
}
