#pragma once
#ifndef _UNISTD_H_
#define _UNISTD_H_

#include <sys/cdefs.h>

#ifndef __kernel__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

BEGIN_DECLS

ssize_t read(int fd, void *data, size_t len);
ssize_t write(int fd, const void *data, size_t len);

int unlink(const char *);
int unlinkat(int atfd, const char *);

int readlink(const char *path, char *buffer, size_t len);
int readlinkat(int atfd, const char *path, char *buffer, size_t len);

pid_t fork(void);

pid_t clone0(int (*fn)(void *), void *new_stack, int flags, void *arg);
pid_t clone(int (*fn)(void *), void *new_stack, int flags, void *arg);

pid_t getpid(void);
pid_t gettid(void);

int execve(const char *program, char *const argv[], char *const envp[]);
int execvp(const char *program, char *const argv[]);

int dup2(int, int);
int isatty(int fd);
int pipe(int *pipefds);
int setpgid(int pid, int pgid);

off_t seek(int fd, off_t offset, int whence);
off_t lseek(int fd, off_t offset, int whence);

enum std_filenos {
    STDIN_FILENO = 0,
    STDOUT_FILENO = 1,
    STDERR_FILENO = 2,
};

int sleep(int seconds);

extern const char *optarg;
extern int optind, opterr, optopt;

int getopt(int argc, char *const argv[], const char *optstring);

#ifndef UNISTD_NO_GETCWD
char *getcwd(char *buffer, size_t len);
#endif

int chdir(const char *path);
int fchdir(int fd);
int chdirat(int atfd, const char *path);

unsigned int alarm(unsigned int seconds);

END_DECLS

#endif // ifndef __kernel__

#endif // _UNISTD_H_
