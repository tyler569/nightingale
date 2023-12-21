#include <assert.h>
#include <ng/cpu.h>
#include <ng/spalloc.h>
#include <ng/syscalls.h>
#include <ng/tests.h>

void run_spalloc_test()
{
    // validate spalloc working
    struct testing {
        int a, b, c, d, e, f, g, h;
    };
    spalloc<testing> foobar { 100 };

    auto *first = foobar.emplace(10);
    auto *second = foobar.emplace(11);

    assert(first->a == 10);

    first->g = 1;
    foobar.free(first);
    assert(first->g != 1); // poison
    assert(second->a == 11);

    auto *re_first = foobar.emplace();
    assert(re_first == first);
}

namespace fs3 {
void test();
}
void cpp_test();
void sync_mt_test();

void run_all_tests()
{
    run_spalloc_test();
    cpp_test();
    fs3::test();
    sync_mt_test();
    printf("ng: all tests passed!\n");
}

sysret sys_test_args(int a1, int a2, int a3, int a4, int a5, int a6)
{
    return a1 + a2 + a3 + a4 + a5 + a6;
}
