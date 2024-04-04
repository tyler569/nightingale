#include <assert.h>
#include <ng/sync.h>
#include <ng/thread.h>

void sync_test_controller(void *);

void run_sync_tests(void) { kthread_create(sync_test_controller, NULL); }

mutex_t join_mutex;
mutex_t new_m;
condvar_t join_cv;
atomic_int n_threads;

int unsynchronized = 0;
atomic_int synchronized = 0;

const long loops = 10000000;
const long print = loops / 10;

void sync_thread(void *);

void sync_test_controller(void *_) {
	mutex_init(&new_m);
	mutex_init(&join_mutex);
	cv_init(&join_cv);

	int n = 3;
	atomic_store(&n_threads, n);
	for (int i = 0; i < n; i++)
		kthread_create(sync_thread, NULL);

	while (atomic_load(&n_threads) > 0) {
		mutex_lock(&join_mutex);
		cv_wait(&join_cv, &join_mutex);
	}

	assert(synchronized == loops * n);
	// assert(unsynchronized == loops * 2);

	printf("unsync: %i\n", unsynchronized);
	printf("  sync: %i\n", synchronized);
	kthread_exit();
}

void sync_thread(void *_) {
	for (int i = 0; i < loops; i++)
		unsynchronized++;

	for (int i = 0; i < loops; i++) {
		mutex_lock(&new_m);
		synchronized++;
		mutex_unlock(&new_m);
	}
	atomic_fetch_sub(&n_threads, 1);
	cv_signal(&join_cv);
	kthread_exit();
}
