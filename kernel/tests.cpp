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
    spalloc foobar {};
    sp_init(&foobar, testing);

    auto *first = (testing *)sp_alloc(&foobar);
    assert(first == foobar.region);
    first->a = 10;

    auto *second = (testing *)sp_alloc(&foobar);
    assert(second == sp_at(&foobar, 1));
    second->a = 11;

    assert(first->a == 10);

    first->g = 1;
    sp_free(&foobar, first);
    assert(first->g != 1); // poison
    assert(second->a == 11);

    auto *re_first = (testing *)sp_alloc(&foobar);
    assert(re_first == first);

    assert(foobar.capacity == 0x10000);
    assert(foobar.count == 2);
}

void run_all_tests()
{
    run_spalloc_test();
    kthread_create(test_kernel_thread, (void *)"get a cat");
    printf("ng: all tests passed!\n");
}
