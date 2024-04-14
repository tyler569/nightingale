#include <ng/chacha20.h>
#include <string.h>

static uint32_t rol(uint32_t v, int n) { return (v << n) | (v >> (32 - n)); }

#define A state->n[a]
#define B state->n[b]
#define C state->n[c]
#define D state->n[d]

typedef struct chacha20_state cc2s;

static void cc_quarter_round(cc2s *state, int a, int b, int c, int d) {
	// clang-format off
    // formatting is from the spec
    A += B; D ^= A; D = rol(D, 16);
    C += D; B ^= C; B = rol(B, 12);
    A += B; D ^= A; D = rol(D, 8);
    C += D; B ^= C; B = rol(B, 7);
	// clang-format on
}

static void cc_block(cc2s *state) {
	for (int i = 0; i < 10; i++) {
		cc_quarter_round(state, 0, 4, 8, 12);
		cc_quarter_round(state, 1, 5, 9, 13);
		cc_quarter_round(state, 2, 6, 10, 14);
		cc_quarter_round(state, 3, 7, 11, 15);
		cc_quarter_round(state, 0, 5, 10, 15);
		cc_quarter_round(state, 1, 6, 11, 12);
		cc_quarter_round(state, 2, 7, 8, 13);
		cc_quarter_round(state, 3, 4, 9, 14);
	}
}

static void cc_add(cc2s *state, struct chacha20_state *add) {
	for (int i = 0; i < 16; i++) {
		state->n[i] += add->n[i];
	}
}

struct chacha20_state chacha20_init(
	const char key[static 32], const char nonce[static 12], uint32_t count) {
	struct chacha20_state state = { {
		0x61707865,
		0x3320646e,
		0x79622d32,
		0x6b206574,
	} };

	memcpy(state.n + 4, key, 8 * sizeof(uint32_t));
	state.n[12] = count;
	memcpy(state.n + 13, nonce, 3 * sizeof(uint32_t));

	return state;
}

void chacha20_read(struct chacha20_state *state, char *buffer, size_t len) {
	size_t output = 0;
	do {
		struct chacha20_state conv = *state;
		cc_block(&conv);
		cc_add(&conv, state);

		size_t n_copy = len - output > 64 ? 64 : len - output;
		memcpy(buffer + output, &conv, n_copy);
		output += n_copy;

		state->n[12] += 1;
	} while (output < len);
}
