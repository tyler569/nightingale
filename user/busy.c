#include <ctype.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
	printf("this is a busy loop\n");
	if (argc < 2) {
		printf("provide an argument for how many times\n");
		return 1;
	}

	long count = atoi(argv[1]);

	printf("looping for %li count\n", count);

	bool do_yield = false;

	if (argc >= 3) {
		if (strcmp(argv[2], "yield") == 0) {
			do_yield = true;
			printf("and yielding each time\n");
		}
	}

	for (int i = 0; i < count; i++) {
		if (do_yield)
			yield();
	}

	return 0;
}
