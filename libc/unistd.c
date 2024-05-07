#define UNISTD_NO_GETCWD 1

#include <fcntl.h>
#include <nightingale.h>
#include <stdio.h>
#include <sys/ttyctl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int isatty(int fd) { return ttyctl(fd, TTY_ISTTY, 0); }

int execvp(const char *path, char *const argv[]) {
	return execve(path, argv, nullptr);
}

off_t lseek(int fd, off_t offset, int whence) {
	return seek(fd, offset, whence);
}

int sleep(int seconds) { return sleepms(seconds * 1000); }

pid_t wait(int *status) { return waitpid(0, status, 0); }

pid_t clone(int (*fn)(void *), void *new_stack, int flags, void *arg) {
	return clone0(fn, new_stack, flags, arg);
}

int load_module(int fd) {
	int loadmod(int fd);
	return loadmod(fd);
}

int chdir(const char *path) { return chdirat(AT_FDCWD, path); }
int fchdir(int fd) { return chdirat(fd, ""); }
