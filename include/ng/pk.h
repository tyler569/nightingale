#pragma once

#include <stdint.h>

#define ETH_MTU 1518

struct pk {
	uint8_t data[ETH_MTU];

	uint16_t len;
	uint16_t l2_offset;
	uint16_t l3_offset;
	uint16_t l4_offset;

	// The interface on which the pk was received, null if it was
	// generated locally.
	struct net_if *origin_if;

	// The next pk containing data for the same frame.
	struct pk *next_in_frame;

	// The next pk on a queue or list, including the free list.
	struct pk *queue_next;
};

struct pk *pk_alloc();
void pk_free(struct pk *pk);

void pk_drop(struct pk *pk);
void pk_reject(struct pk *pk);
