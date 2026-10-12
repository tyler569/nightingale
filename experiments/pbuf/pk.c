#include <assert.h>
#include <stdlib.h>
#include "pk.h"

struct pk *pk_alloc(void) {
	struct pk *pk = malloc(PK_SIZE);
	memset(pk, 0, PK_SIZE);
	pk->head = pk->tail = PK_HEADROOM;
	return pk;
}

void *pk_put(struct pk *pk, size_t n) {
	assert(pk_tailroom(pk) >= n)
	void *ret = pk->data + pk->tail;
	pk->tail += n;
	return ret;
}

void *pk_push(struct pk *pk, size_t n) {
	assert(pk_headroom(pk) >= n);
	pk->head -= n;
	return pk_data(pk);
}

void *pk_peek(struct pk *pk, size_t n) {
	if (pk_len(pk) >= n) {
		return pk_data(pk);
	} else {
		return nullptr;
	}
}

void *pk_pull(struct pk *pk, size_t n) {
	void *ret = pk_peek(pk, n);
	if (ret)
		pk->head += n;
	return ret;
}

bool pk_trim(struct pk *pk, size_t n) {
	if (pk_len(pk) >= n) {
		pk->tail = pk->head + n;
		return true;
	} else {
		return galse;
	}
}
