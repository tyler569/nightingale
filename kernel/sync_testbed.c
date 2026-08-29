#include <assert.h>
#include <ng/sync.h>
#include <ng/thread.h>

void sync_test_controller(void *);

void run_sync_tests() {
	kthread_create(sync_test_controller, nullptr);
}

mutex_t join_mutex;
mutex_t new_m;
cv_t join_cv;
constexpr int spawn_threads = 10;
int n_threads = spawn_threads;

int unsynchronized = 0;
atomic_int synchronized = 0;

constexpr static long loops = 10'000;

void sync_thread(void *);

void sync_test_controller(void *) {
	mutex_init(&new_m);
	mutex_init(&join_mutex);
	cv_init(&join_cv);

	for (int i = 0; i < spawn_threads; i++)
		kthread_create(sync_thread, nullptr);

	mutex_lock(&join_mutex);

	while (n_threads > 0)
		cv_wait(&join_cv, &join_mutex);

	mutex_unlock(&join_mutex);

	assert(synchronized == loops * spawn_threads);
	// assert(unsynchronized == loops * n);

	// printf("unsync: %i\n", unsynchronized);
	// printf("  sync: %i\n", synchronized);
	kthread_exit();
}

void sync_thread(void *) {
	for (int i = 0; i < loops; i++)
		unsynchronized++;

	for (int i = 0; i < loops; i++) {
		mutex_lock(&new_m);
		synchronized++;
		mutex_unlock(&new_m);
	}

	mutex_lock(&join_mutex);
	n_threads--;

	if (n_threads == 0)
		cv_wake_all_locked(&join_cv);

	mutex_unlock(&join_mutex);

	kthread_exit();
}
