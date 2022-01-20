#include <basic.h>
#include <ng/panic.h>
#include <ng/sync.h>
#include <ng/thread.h>

void sync_test_controller(void *);

void run_sync_tests(void) {
    kthread_create(sync_test_controller, NULL);
}

mutex_t join_a_mutex;
mutex_t join_b_mutex;
mutex_t new_m;
struct condvar join_a = CV_INIT(join_a);
struct condvar join_b = CV_INIT(join_b);

atomic_int unsynchronized = 0;
atomic_int synchronized = 0;

const long loops = 100000;
const long print = loops / 10;

void sync_thread_a(void *);
void sync_thread_b(void *);

void sync_test_controller(void *_) {
    mutex_init(&new_m);
    mutex_init(&join_a_mutex);
    mutex_init(&join_b_mutex);

    kthread_create(sync_thread_a, NULL);
    kthread_create(sync_thread_b, NULL);

    mutex_lock(&join_a_mutex);
    cv_wait(&join_a, &join_a_mutex);
    mutex_unlock(&join_a_mutex);

    mutex_lock(&join_b_mutex);
    cv_wait(&join_b, &join_b_mutex);
    mutex_unlock(&join_b_mutex);

    assert(synchronized == loops * 2);
    // assert(unsynchronized == loops * 2);

    // printf("unsync: %i\n", unsynchronized);
    // printf("  sync: %i\n", synchronized);
    kthread_exit();
}

void sync_thread_a(void *_) {
    for (int i = 0; i < loops; i++) {
        mutex_lock(&new_m);
        synchronized++;
        mutex_unlock(&new_m);
    }
    cv_signal(&join_a);
    kthread_exit();
}

void sync_thread_b(void *_) {
    for (int i = 0; i < loops; i++) {
        mutex_lock(&new_m);
        synchronized++;
        mutex_unlock(&new_m);
    }
    cv_signal(&join_b);
    kthread_exit();
}
