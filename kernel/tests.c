#include <basic.h>
#include <ng/spalloc.h>
#include <assert.h>

void run_sync_tests(void);

void run_all_tests() {
        run_sync_tests();

        {
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
}

