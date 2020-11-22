#include <basic.h>
#include <stddef.h>
#include <stdint.h>

static uint64_t random_pool[4];

void rand_add_entropy(uint64_t entropy) {
    // TODO: Mersenne twister

    random_pool[0] ^= entropy;
    random_pool[1] ^= (entropy * 17);
    random_pool[2] ^= (entropy * 31);
    random_pool[3] ^= (entropy * 59);

    for (int i = 0; i < 5; i++) {
        for (int ix = 0; ix < 4; ix++) {
            random_pool[ix] += 5 + i;
            random_pool[ix] ^= random_pool[(ix + 1) % 4];
        }
    }
}

int32_t rand_get() {
    int32_t result = random_pool[2] & 0xFFFFFFFF;
    rand_add_entropy(result);
    return result;
}
