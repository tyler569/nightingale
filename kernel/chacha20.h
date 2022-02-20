#pragma once
#include <stdint.h>

struct chacha20_state {
    uint32_t n[16];
};

struct chacha20_state init(
    char key[static 32],
    char nonce[static 12],
    uint32_t count
);

void chacha20_keystream(struct chacha20_state *state, char *buffer, size_t len);
