#include <ng/early_alloc.h>
#include <stddef.h>
#include <sys/cdefs.h>

static constexpr size_t early_memory_size = 128 * 1024;

static char early_memory[early_memory_size];
static size_t waterline;

void *early_alloc(size_t len) {
	void *ptr = early_memory + waterline;
	waterline += len;
	return ptr;
}

void *early_alloc_aligned(size_t len, size_t alignment) {
	waterline = ROUND_UP(waterline, alignment);
	return early_alloc(len);
}
