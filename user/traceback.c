#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

void fail(const char *);

char buffer[2048] = { 0 };

int main(int argc, char **argv) {
	char path[256];

	if (argc != 2) {
		fprintf(stderr, "An argument is required - traceback [pid]\n");
		return 1;
	}
	pid_t tid = strtol(argv[1], NULL, 10);
	snprintf(path, 256, "/proc/%i/stack", tid);

	int fd = open(path, O_RDONLY);
	if (fd < 0)
		fail("open");

	int err = read(fd, buffer, sizeof(buffer));
	if (err < 0)
		perror("traceback");

	printf("%s", buffer);
	return 0;
}

void fail(const char *message) {
	perror(message);
	exit(1);
}
