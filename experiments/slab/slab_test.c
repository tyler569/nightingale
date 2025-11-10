#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "slab.h"

void test_single_alloc() {
	printf("Test: Single allocation\n");
	struct slab_cache cache;
	slab_cache_init(&cache, 32);

	void *obj = slab_alloc(&cache);
	assert(obj != NULL);

	memset(obj, 0xAA, 32);

	printf("  PASS\n\n");
}

void test_multiple_allocs() {
	printf("Test: Multiple allocations\n");
	struct slab_cache cache;
	slab_cache_init(&cache, 64);

	void *objs[10];
	for (int i = 0; i < 10; i++) {
		objs[i] = slab_alloc(&cache);
		assert(objs[i] != NULL);
		memset(objs[i], i, 64);
	}

	// Verify all pointers are different
	for (int i = 0; i < 10; i++) {
		for (int j = i + 1; j < 10; j++) {
			assert(objs[i] != objs[j]);
		}
	}

	printf("  PASS\n\n");
}

void test_alloc_free_alloc() {
	printf("Test: Allocate, free, allocate again\n");
	struct slab_cache cache;
	slab_cache_init(&cache, 128);

	void *obj1 = slab_alloc(&cache);
	assert(obj1 != NULL);

	slab_free(&cache, obj1);

	void *obj2 = slab_alloc(&cache);
	assert(obj2 != NULL);

	printf("  PASS\n\n");
}

void test_many_allocs() {
	printf("Test: Many allocations (force multiple slabs)\n");
	struct slab_cache cache;
	slab_cache_init(&cache, 256);

	void *objs[100];
	for (int i = 0; i < 100; i++) {
		objs[i] = slab_alloc(&cache);
		assert(objs[i] != NULL);
		*(int *)objs[i] = i;
	}

	// Verify data integrity
	for (int i = 0; i < 100; i++) {
		assert(*(int *)objs[i] == i);
	}

	printf("  PASS\n\n");
}

void test_interleaved_alloc_free() {
	printf("Test: Interleaved allocations and frees\n");
	struct slab_cache cache;
	slab_cache_init(&cache, 48);

	void *obj1 = slab_alloc(&cache);
	void *obj2 = slab_alloc(&cache);
	void *obj3 = slab_alloc(&cache);

	assert(obj1 != NULL && obj2 != NULL && obj3 != NULL);

	slab_free(&cache, obj2);

	void *obj4 = slab_alloc(&cache);
	assert(obj4 != NULL);

	void *obj5 = slab_alloc(&cache);
	assert(obj5 != NULL);

	slab_free(&cache, obj1);
	slab_free(&cache, obj3);
	slab_free(&cache, obj4);
	slab_free(&cache, obj5);

	printf("  PASS\n\n");
}

void test_small_objects() {
	printf("Test: Small object size (8 bytes)\n");
	struct slab_cache cache;
	slab_cache_init(&cache, 8);

	void *objs[50];
	for (int i = 0; i < 50; i++) {
		objs[i] = slab_alloc(&cache);
		assert(objs[i] != NULL);
	}

	printf("  PASS\n\n");
}

void test_large_objects() {
	printf("Test: Large object size (2048 bytes)\n");
	struct slab_cache cache;
	slab_cache_init(&cache, 2048);

	void *objs[10];
	for (int i = 0; i < 10; i++) {
		objs[i] = slab_alloc(&cache);
		assert(objs[i] != NULL);
		memset(objs[i], i, 2048);
	}

	printf("  PASS\n\n");
}

void test_free_all_then_realloc() {
	printf("Test: Free all objects then reallocate\n");
	struct slab_cache cache;
	slab_cache_init(&cache, 96);

	void *objs[20];
	for (int i = 0; i < 20; i++) {
		objs[i] = slab_alloc(&cache);
		assert(objs[i] != NULL);
	}

	for (int i = 0; i < 20; i++) {
		slab_free(&cache, objs[i]);
	}

	for (int i = 0; i < 20; i++) {
		objs[i] = slab_alloc(&cache);
		assert(objs[i] != NULL);
	}

	printf("  PASS\n\n");
}

void test_different_sizes() {
	printf("Test: Multiple caches with different sizes\n");

	struct slab_cache cache32, cache64, cache128;
	slab_cache_init(&cache32, 32);
	slab_cache_init(&cache64, 64);
	slab_cache_init(&cache128, 128);

	void *obj32 = slab_alloc(&cache32);
	void *obj64 = slab_alloc(&cache64);
	void *obj128 = slab_alloc(&cache128);

	assert(obj32 != NULL);
	assert(obj64 != NULL);
	assert(obj128 != NULL);

	memset(obj32, 0x32, 32);
	memset(obj64, 0x64, 64);
	memset(obj128, 0x80, 128);

	printf("  PASS\n\n");
}

void test_fill_slab_completely() {
	printf("Test: Fill a slab completely\n");
	struct slab_cache cache;
	slab_cache_init(&cache, 32);

	void *objs[200];
	int allocated = 0;

	for (int i = 0; i < 200; i++) {
		objs[i] = slab_alloc(&cache);
		if (objs[i] == NULL)
			break;
		allocated++;
	}

	printf("  Allocated %d objects\n", allocated);
	assert(allocated > 0);

	printf("  PASS\n\n");
}

void test_tiny_sizes() {
	printf("Test: Tiny object sizes (1-8 bytes)\n");

	for (size_t size = 1; size <= 8; size++) {
		struct slab_cache cache;
		slab_cache_init(&cache, size);

		void *objs[20];
		for (int i = 0; i < 20; i++) {
			objs[i] = slab_alloc(&cache);
			assert(objs[i] != NULL);
			memset(objs[i], i, size);
		}

		// Verify all different
		for (int i = 0; i < 20; i++) {
			for (int j = i + 1; j < 20; j++) {
				assert(objs[i] != objs[j]);
			}
		}
	}

	printf("  PASS\n\n");
}

void test_alignment_boundary_sizes() {
	printf("Test: Sizes around alignment boundaries\n");

	size_t test_sizes[]
		= { 9, 15, 16, 17, 23, 24, 31, 32, 33, 47, 48, 49, 63, 64, 65 };

	for (size_t i = 0; i < sizeof(test_sizes) / sizeof(test_sizes[0]); i++) {
		struct slab_cache cache;
		slab_cache_init(&cache, test_sizes[i]);

		void *objs[10];
		for (int j = 0; j < 10; j++) {
			objs[j] = slab_alloc(&cache);
			assert(objs[j] != NULL);
			memset(objs[j], 0xFF, test_sizes[i]);
		}

		// Verify alignment (should be 8 or 16 byte aligned)
		for (int j = 0; j < 10; j++) {
			uintptr_t addr = (uintptr_t)objs[j];
			assert((addr & 7) == 0); // At least 8-byte aligned
		}
	}

	printf("  PASS\n\n");
}

void test_very_large_objects() {
	printf("Test: Very large object sizes\n");

	size_t test_sizes[] = { 4096, 8192, 16384 };

	for (size_t i = 0; i < sizeof(test_sizes) / sizeof(test_sizes[0]); i++) {
		struct slab_cache cache;
		slab_cache_init(&cache, test_sizes[i]);

		void *obj = slab_alloc(&cache);
		assert(obj != NULL);

		// Write to entire object to ensure it's valid memory
		memset(obj, 0xAB, test_sizes[i]);
	}

	printf("  PASS\n\n");
}

void test_free_in_reverse() {
	printf("Test: Free in reverse order (LIFO)\n");
	struct slab_cache cache;
	slab_cache_init(&cache, 40);

	void *objs[30];
	for (int i = 0; i < 30; i++) {
		objs[i] = slab_alloc(&cache);
		assert(objs[i] != NULL);
		*(int *)objs[i] = i;
	}

	// Free in reverse order
	for (int i = 29; i >= 0; i--) {
		slab_free(&cache, objs[i]);
	}

	// Reallocate and verify
	for (int i = 0; i < 30; i++) {
		objs[i] = slab_alloc(&cache);
		assert(objs[i] != NULL);
	}

	printf("  PASS\n\n");
}

void test_alternating_alloc_free() {
	printf("Test: Alternating allocate and free pattern\n");
	struct slab_cache cache;
	slab_cache_init(&cache, 56);

	void *obj1, *obj2, *obj3;

	for (int i = 0; i < 50; i++) {
		obj1 = slab_alloc(&cache);
		obj2 = slab_alloc(&cache);
		obj3 = slab_alloc(&cache);

		assert(obj1 != NULL && obj2 != NULL && obj3 != NULL);

		slab_free(&cache, obj1);
		slab_free(&cache, obj3);

		obj1 = slab_alloc(&cache);
		assert(obj1 != NULL);

		slab_free(&cache, obj1);
		slab_free(&cache, obj2);
	}

	printf("  PASS\n\n");
}

void test_random_alloc_free_pattern() {
	printf("Test: Random allocation/free pattern\n");
	struct slab_cache cache;
	slab_cache_init(&cache, 72);

	void *objs[50] = { 0 };

	// Pseudo-random pattern of allocs and frees
	for (int round = 0; round < 10; round++) {
		// Allocate some
		for (int i = 0; i < 50; i++) {
			if (objs[i] == NULL && (i * 17 + round) % 3 == 0) {
				objs[i] = slab_alloc(&cache);
				assert(objs[i] != NULL);
			}
		}

		// Free some
		for (int i = 0; i < 50; i++) {
			if (objs[i] != NULL && (i * 13 + round) % 4 == 0) {
				slab_free(&cache, objs[i]);
				objs[i] = NULL;
			}
		}
	}

	// Clean up remaining
	for (int i = 0; i < 50; i++) {
		if (objs[i] != NULL) {
			slab_free(&cache, objs[i]);
		}
	}

	printf("  PASS\n\n");
}

void test_data_integrity_after_many_operations() {
	printf("Test: Data integrity after many operations\n");
	struct slab_cache cache;
	slab_cache_init(&cache, 80);

	void *objs[25];

	// Allocate and write unique values
	for (int i = 0; i < 25; i++) {
		objs[i] = slab_alloc(&cache);
		assert(objs[i] != NULL);

		// Write pattern
		for (int j = 0; j < 20; j++) {
			((int *)objs[i])[j] = i * 1000 + j;
		}
	}

	// Verify integrity
	for (int i = 0; i < 25; i++) {
		for (int j = 0; j < 20; j++) {
			assert(((int *)objs[i])[j] == i * 1000 + j);
		}
	}

	// Free half
	for (int i = 0; i < 25; i += 2) {
		slab_free(&cache, objs[i]);
		objs[i] = NULL;
	}

	// Verify remaining data still intact
	for (int i = 1; i < 25; i += 2) {
		for (int j = 0; j < 20; j++) {
			assert(((int *)objs[i])[j] == i * 1000 + j);
		}
	}

	printf("  PASS\n\n");
}

void test_stress_single_object_reuse() {
	printf("Test: Stress test - single object reuse\n");
	struct slab_cache cache;
	slab_cache_init(&cache, 24);

	void *obj;
	for (int i = 0; i < 1000; i++) {
		obj = slab_alloc(&cache);
		assert(obj != NULL);
		*(int *)obj = i;
		assert(*(int *)obj == i);
		slab_free(&cache, obj);
	}

	printf("  PASS\n\n");
}

void test_odd_sizes() {
	printf("Test: Odd/unusual object sizes\n");

	size_t odd_sizes[]
		= { 1, 3, 5, 7, 11, 13, 17, 19, 23, 29, 37, 41, 53, 67, 83, 97 };

	for (size_t i = 0; i < sizeof(odd_sizes) / sizeof(odd_sizes[0]); i++) {
		struct slab_cache cache;
		slab_cache_init(&cache, odd_sizes[i]);

		void *objs[15];
		for (int j = 0; j < 15; j++) {
			objs[j] = slab_alloc(&cache);
			assert(objs[j] != NULL);
		}

		for (int j = 0; j < 15; j++) {
			slab_free(&cache, objs[j]);
		}
	}

	printf("  PASS\n\n");
}

int main() {
	printf("=== Slab Allocator Test Suite ===\n\n");

	// Original tests
	test_single_alloc();
	test_multiple_allocs();
	test_alloc_free_alloc();
	test_many_allocs();
	test_interleaved_alloc_free();
	test_small_objects();
	test_large_objects();
	test_free_all_then_realloc();
	test_different_sizes();
	test_fill_slab_completely();

	// New tests for stored_size and edge cases
	test_tiny_sizes();
	test_alignment_boundary_sizes();
	test_very_large_objects();
	test_free_in_reverse();
	test_alternating_alloc_free();
	test_random_alloc_free_pattern();
	test_data_integrity_after_many_operations();
	test_stress_single_object_reuse();
	test_odd_sizes();

	printf("=== All tests completed ===\n");

	return 0;
}
