#include <nightingale.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void sig_handle(int signal) {
	printf("bg: received signal %i\n", signal);
}

int main() {
	int child = fork();

	if (child) {
		printf("bg: child is %i\n", child);
		return EXIT_SUCCESS;
	}

	signal(SIGINT, sig_handle);

	while (1) {
		sleep(1);
	}
}
