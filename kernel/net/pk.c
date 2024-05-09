#include <netinet/debug.h>
#include <ng/limine.h>
#include <ng/net.h>
#include <ng/pk.h>
#include <ng/pmm.h>
#include <ng/sync.h>
#include <stdio.h>

static spin_lock_t pk_lock = {};
static struct pk *pk_free_list_head = nullptr;

struct pk *pk_alloc() {
	spin_lock(&pk_lock);

	struct pk *pk;
	if (pk_free_list_head) {
		pk = pk_free_list_head;
		pk_free_list_head = pk_free_list_head->queue_next;
	} else {
		// Allocate a 4KB page and split it into two 2KB buffers.
		phys_addr_t page = pm_alloc();
		virt_addr_t p = page | limine_hhdm();

		pk = (struct pk *)p;
		struct pk *pk2 = (struct pk *)(p + 2048);

		pk_free_list_head = pk2;
	}

	spin_unlock(&pk_lock);
	return pk;
}

void pk_free(struct pk *pk) {
	spin_lock(&pk_lock);

	pk->queue_next = pk_free_list_head;
	pk_free_list_head = pk;

	spin_unlock(&pk_lock);
}

void pk_reject(struct pk *pk) {
	printf("Rejecting packet\n");
	net_debug_pk(pk);
	pk_free(pk);
}

void pk_drop(struct pk *pk) {
	printf("Dropping packet\n");
	pk_free(pk);
}

void pk_done(struct pk *pk) { pk_free(pk); }
