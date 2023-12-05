#pragma once

#include "sys/cdefs.h"
#include <stdint.h>
#include <stdlib.h>

BEGIN_DECLS

struct chacha20_state {
    uint32_t n[16];
};

#ifdef __cplusplus
chacha20_state init(const char key[32], const char nonce[12], uint32_t count);
#else
struct chacha20_state init(
    const char key[static 32], const char nonce[static 12], uint32_t count);
#endif

void chacha20_keystream(struct chacha20_state *state, char *buffer, size_t len);

END_DECLS