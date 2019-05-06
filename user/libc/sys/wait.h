
#ifndef _WAIT_H_
#define _WAIT_H_

#include <ng/syscall_consts.h>

#define WEXITSTATUS(stat_val) 0
#define WIFEXITED(stat_val) 0
#define WIFSIGNALED(stat_val) 0
#define WTERMSIG(stat_val) 0

int waitpid(pid_t pid, int *status, int options);

#endif
