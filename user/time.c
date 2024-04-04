#include <errno.h>
#include <nightingale.h>
#include <stdio.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char **argv) {
	if (argc < 2) {
		printf("usage: time [command]");
		exit(0);
	}
	long time = xtime();
	uint64_t tsc = rdtsc();

	int child = fork();
	int exit_status;
	if (child == 0) {
		int e = execve(argv[1], argv + 1, NULL);
		perror("errno()");
		exit(1);
	} else {
		waitpid(child, &exit_status, 0);
	}
	uint64_t end_tsc = rdtsc();
	long end_time = xtime();

	printf("time: %li ms\n", (end_time - time) * (1000 / CLOCKS_PER_SEC));
	printf("tsc time: %lu clocks\n", end_tsc - tsc);
	return exit_status;
}
