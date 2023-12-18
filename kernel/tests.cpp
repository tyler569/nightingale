#include "nx/print.h"
#include <assert.h>
#include <ng/cpu.h>
#include <ng/spalloc.h>
#include <ng/tests.h>
#include <ng/thread.h>
#include <stdnoreturn.h>

void run_sync_tests();

void test_kernel_thread(void *arg)
{
    const char *message = static_cast<const char *>(arg);
    assert(strcmp(message, "get a cat") == 0);
    kthread_exit();
}

void run_spalloc_test()
{
    // validate spalloc working
    struct testing {
        int a, b, c, d, e, f, g, h;
    };
    spalloc<testing> foobar { 100 };

    auto *first = foobar.emplace(10);
    auto *second = foobar.emplace(11);

    nx::print("% %\n", first, second);

    assert(first->a == 10);

    first->g = 1;
    foobar.free(first);
    assert(first->g != 1); // poison
    assert(second->a == 11);

    auto *re_first = foobar.emplace();
    nx::print("%\n", re_first);
    assert(re_first == first);
}

void run_all_tests()
{
    run_spalloc_test();
    kthread_create(test_kernel_thread, (void *)"get a cat");
    printf("ng: all tests passed!\n");
}
