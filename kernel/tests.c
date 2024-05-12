#include <assert.h>
#include <ng/cpu.h>
#include <ng/spalloc.h>
#include <ng/tests.h>
#include <ng/thread.h>

void run_sync_tests();

[[noreturn]] void test_kernel_thread(void *arg) {
	const char *message = arg;
	assert(strcmp(arg, "get a cat") == 0);
	kthread_exit();
}

void run_all_tests() { kthread_create(test_kernel_thread, "get a cat"); }
