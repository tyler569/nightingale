
#ifndef _WAIT_H_
#define _WAIT_H_

#include <ng/syscall_consts.h>

int waitpid(pid_t pid, int *status, int options);

#endif
