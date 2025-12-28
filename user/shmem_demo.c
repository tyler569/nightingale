#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// Shared memory structure for communication
struct shared_data {
	int counter;
	int parent_value;
	int child_value;
	char message[128];
};

int main() {
	printf("=== Shared Memory Demo ===\n");
	printf("Testing MAP_SHARED with fork()\n\n");

	// Create a shared memory mapping
	size_t size = sizeof(struct shared_data);
	struct shared_data *shared = mmap(
		NULL,                           // Let kernel choose address
		size,                           // Size of mapping
		PROT_READ | PROT_WRITE,        // Read/write access
		MAP_SHARED | MAP_ANONYMOUS,    // Shared, not backed by file
		-1,                            // No file descriptor
		0                              // No offset
	);

	if (shared == MAP_FAILED) {
		printf("ERROR: mmap failed!\n");
		return 1;
	}

	printf("✓ Successfully created shared memory at %p\n", shared);
	printf("  Size: %zu bytes\n\n", size);

	// Initialize shared memory
	shared->counter = 0;
	shared->parent_value = 0;
	shared->child_value = 0;
	strcpy(shared->message, "Initial message from parent");

	printf("Parent initializing shared memory:\n");
	printf("  counter = %d\n", shared->counter);
	printf("  message = \"%s\"\n\n", shared->message);

	// Fork a child process
	pid_t pid = fork();

	if (pid < 0) {
		printf("ERROR: fork failed!\n");
		munmap(shared, size);
		return 1;
	}

	if (pid == 0) {
		// === CHILD PROCESS ===
		printf("Child process (pid=%d) started\n", getpid());
		printf("Child can see shared memory at %p\n\n", shared);

		// Read initial values set by parent
		printf("Child reading initial values:\n");
		printf("  counter = %d\n", shared->counter);
		printf("  message = \"%s\"\n\n", shared->message);

		// Sleep briefly to let parent go first
		sleep(1);

		// Increment counter
		shared->counter++;
		printf("Child incremented counter to %d\n", shared->counter);

		// Write child's value
		shared->child_value = 42;
		printf("Child wrote child_value = %d\n", shared->child_value);

		// Update message
		strcpy(shared->message, "Hello from child process!");
		printf("Child updated message\n\n");

		// Wait a bit, then read parent's value
		sleep(1);

		printf("Child reading values after parent write:\n");
		printf("  counter = %d\n", shared->counter);
		printf("  parent_value = %d\n", shared->parent_value);
		printf("  child_value = %d\n", shared->child_value);
		printf("  message = \"%s\"\n\n", shared->message);

		printf("Child process exiting\n");
		exit(0);

	} else {
		// === PARENT PROCESS ===
		printf("Parent process (pid=%d) forked child (pid=%d)\n\n", getpid(), pid);

		// Wait a bit for child to read initial values
		sleep(1);

		// Read what child wrote
		printf("Parent reading after child write:\n");
		printf("  counter = %d\n", shared->counter);
		printf("  child_value = %d\n", shared->child_value);
		printf("  message = \"%s\"\n\n", shared->message);

		// Increment counter
		shared->counter++;
		printf("Parent incremented counter to %d\n", shared->counter);

		// Write parent's value
		shared->parent_value = 100;
		printf("Parent wrote parent_value = %d\n", shared->parent_value);

		// Update message
		strcpy(shared->message, "Hello from parent process!");
		printf("Parent updated message\n\n");

		// Wait for child to finish
		int status;
		waitpid(pid, &status, 0);
		printf("Parent: child process exited with status %d\n\n", status);

		// Final check of shared memory
		printf("Parent reading final state:\n");
		printf("  counter = %d (should be 2)\n", shared->counter);
		printf("  parent_value = %d\n", shared->parent_value);
		printf("  child_value = %d\n", shared->child_value);
		printf("  message = \"%s\"\n\n", shared->message);

		// Cleanup
		if (munmap(shared, size) == 0) {
			printf("✓ Successfully unmapped shared memory\n");
		} else {
			printf("ERROR: munmap failed!\n");
		}

		printf("\n=== Demo Complete ===\n");
		printf("If counter = 2, shared memory worked correctly!\n");
	}

	return 0;
}
