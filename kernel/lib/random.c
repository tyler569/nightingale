#include <ng/chacha20.h>
#include <ng/x86/cpu.h>
#include <stdatomic.h>
#include <string.h>

#define POOL_BITS 7
#define POOL_SIZE (1 << POOL_BITS)
#define POOL_MASK (POOL_SIZE - 1)

static char random_pool[POOL_SIZE];
static char random_scratch_pool[POOL_SIZE];

void random_write(const char *buffer, size_t len) {
	for (size_t i = 0; i < len; i++) {
		random_pool[i & POOL_MASK] ^= buffer[i];
	}

	struct chacha20_state state
		= chacha20_init(random_pool, (char *)random_pool + 32, 0);

	memcpy(state.n + 4, buffer, MIN(len, 48));

	chacha20_read(&state, random_pool, POOL_SIZE);
	for (int j = 0; j < POOL_SIZE; j++)
		random_pool[j] ^= random_scratch_pool[j];
}

void random_add_boot_randomness() {
	for (int i = 0; i < 100; i++) {
		uint64_t time = rdtsc();
		random_write((char *)&time, 8);
	}
}

atomic_long global_nonce = 1;

size_t random_read(char *buffer, size_t len) {
	long nonce[2] = { 0, 1 };
	nonce[0] = atomic_fetch_add(&global_nonce, 1);
	struct chacha20_state state = chacha20_init(random_pool, (char *)nonce, 1);

	chacha20_read(&state, buffer, len);
	return len;
}
