#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define PAGE_SIZE 4096

void print_separator() {
	printf("----------------------------------------\n");
}

int main() {
	printf("=== mmap/munmap Test Suite ===\n\n");

	// Test 1: Basic anonymous mapping
	print_separator();
	printf("Test 1: Basic MAP_PRIVATE | MAP_ANONYMOUS\n");
	print_separator();

	void *addr1 = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE,
		MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	if (addr1 == MAP_FAILED) {
		printf("✗ FAILED: mmap returned MAP_FAILED\n");
		return 1;
	}

	printf("✓ Mapped memory at %p\n", addr1);

	// Write to it
	char *data = (char *)addr1;
	strcpy(data, "Hello, mmap!");
	printf("✓ Wrote: \"%s\"\n", data);
	printf("✓ Read back: \"%s\"\n", data);

	printf("\n");

	// Test 2: Multiple mappings with gap finding
	print_separator();
	printf("Test 2: Multiple mappings (gap finding)\n");
	print_separator();

	void *addr2 = mmap(NULL, PAGE_SIZE * 2, PROT_READ | PROT_WRITE,
		MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	if (addr2 == MAP_FAILED) {
		printf("✗ FAILED: Second mmap failed\n");
		return 1;
	}

	printf("✓ First mapping:  %p\n", addr1);
	printf("✓ Second mapping: %p\n", addr2);

	if (addr2 > addr1) {
		printf("✓ Second mapping is at higher address (correct)\n");
	}

	void *addr3 = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE,
		MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	if (addr3 == MAP_FAILED) {
		printf("✗ FAILED: Third mmap failed\n");
		return 1;
	}

	printf("✓ Third mapping:  %p\n", addr3);
	printf("\n");

	// Test 3: munmap
	print_separator();
	printf("Test 3: munmap (freeing memory)\n");
	print_separator();

	printf("Unmapping second mapping at %p...\n", addr2);
	if (munmap(addr2, PAGE_SIZE * 2) == 0) {
		printf("✓ munmap succeeded\n");
	} else {
		printf("✗ munmap failed\n");
		return 1;
	}

	// Test 4: Reuse freed space
	printf("Allocating new mapping (should reuse freed space)...\n");
	void *addr4 = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE,
		MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	if (addr4 == MAP_FAILED) {
		printf("✗ FAILED: mmap after munmap failed\n");
		return 1;
	}

	printf("✓ New mapping at %p\n", addr4);

	if (addr4 >= addr2 && addr4 < (void *)((char *)addr2 + PAGE_SIZE * 2)) {
		printf("✓ EXCELLENT: Reused freed address space!\n");
	} else {
		printf("✓ Allocated elsewhere (also valid)\n");
	}

	printf("\n");

	// Test 5: MAP_FIXED
	print_separator();
	printf("Test 5: MAP_FIXED (map at specific address)\n");
	print_separator();

	// First unmap addr1 to free up space
	if (munmap(addr1, PAGE_SIZE) != 0) {
		printf("✗ Failed to unmap addr1\n");
		return 1;
	}

	// Try to map at the exact same address
	void *fixed_addr = mmap(addr1, PAGE_SIZE, PROT_READ | PROT_WRITE,
		MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0);

	if (fixed_addr == MAP_FAILED) {
		printf("✗ MAP_FIXED failed (this might be expected if overlap)\n");
	} else if (fixed_addr == addr1) {
		printf("✓ MAP_FIXED succeeded at requested address %p\n", fixed_addr);
		strcpy((char *)fixed_addr, "Fixed mapping!");
		printf("✓ Data: \"%s\"\n", (char *)fixed_addr);
	} else {
		printf("✗ MAP_FIXED returned wrong address: %p (expected %p)\n",
			fixed_addr, addr1);
	}

	printf("\n");

	// Test 6: MAP_SHARED
	print_separator();
	printf("Test 6: MAP_SHARED\n");
	print_separator();

	void *shared = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE,
		MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	if (shared == MAP_FAILED) {
		printf("✗ MAP_SHARED failed\n");
		return 1;
	}

	printf("✓ MAP_SHARED mapping at %p\n", shared);
	strcpy((char *)shared, "Shared data");
	printf("✓ Wrote to shared memory: \"%s\"\n", (char *)shared);

	printf("\n");

	// Test 7: Invalid operations
	print_separator();
	printf("Test 7: Error handling\n");
	print_separator();

	// Try to munmap with misaligned address
	printf("Testing munmap with misaligned address...\n");
	if (munmap((void *)((char *)shared + 1), PAGE_SIZE) != 0) {
		printf("✓ munmap correctly rejected misaligned address\n");
	} else {
		printf("✗ munmap should have failed but didn't\n");
	}

	// Try to munmap with wrong size
	printf("Testing munmap with wrong size...\n");
	if (munmap(shared, PAGE_SIZE / 2) != 0) {
		printf("✓ munmap correctly rejected size mismatch\n");
	} else {
		printf("✗ munmap should have failed but didn't\n");
	}

	// Try invalid flags
	printf("Testing mmap with invalid flags (neither SHARED nor PRIVATE)...\n");
	void *bad = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE,
		MAP_ANONYMOUS, -1, 0);  // Missing MAP_SHARED or MAP_PRIVATE

	if (bad == MAP_FAILED) {
		printf("✓ mmap correctly rejected invalid flags\n");
	} else {
		printf("✗ mmap should have failed but returned %p\n", bad);
		munmap(bad, PAGE_SIZE);
	}

	printf("\n");

	// Cleanup
	print_separator();
	printf("Cleanup\n");
	print_separator();

	if (fixed_addr != MAP_FAILED) munmap(fixed_addr, PAGE_SIZE);
	munmap(addr3, PAGE_SIZE);
	munmap(addr4, PAGE_SIZE);
	munmap(shared, PAGE_SIZE);

	printf("✓ All mappings cleaned up\n");

	printf("\n");
	print_separator();
	printf("=== All Tests Complete ===\n");
	print_separator();

	return 0;
}
