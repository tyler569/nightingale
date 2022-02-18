#include <basic.h>
#include <stdatomic.h>
#include <string.h>
#include <x86/cpu.h>
#include "chacha20.h"

#define POOL_BITS 8
#define POOL_SIZE (1 << POOL_BITS)
#define POOL_MASK (POOL_SIZE - 1)

char random_pool[256];

void add_to_random(const char *buffer, size_t len) {
    struct chacha20_state state = {{
        0x61707865, 0x3320646e, 0x79622d32, 0x6b206574,
    }};

    memcpy(state.n + 4, buffer, min(len, 32));
    state.n[13] = 0x55;

    char buf[64];
    for (int i = 0; i < 4; i++) {
        chacha20_keystream(&state, buf, 64);
        for (int j = 0; j < 64; j++) {
            random_pool[j + i * 64] ^= buf[j];
        }
    }
}

void random_dance() {
    for (int i = 0; i < 1000; i++) {
        for (int loop = 0; loop < 100 * i; loop++) asm volatile ("");
        uint64_t time = rdtsc();
        add_to_random((char *)&time, 8);
    }
}

atomic_int counter = 1;

size_t get_random(char *buffer, size_t len) {
    atomic_fetch_add(&counter, 1);
    struct chacha20_state state = init(
            random_pool,
            random_pool + 32,
            atomic_load(&counter)
    );
    
    char internal[32];
    chacha20_keystream(&state, internal, 32);
    add_to_random(internal, 32);

    chacha20_keystream(&state, buffer, len);
    return len;
}
