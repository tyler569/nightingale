
#pragma once
#ifndef _SYS_WAIT_H_
#define _SYS_WAIT_H_

#include <ng/syscall_consts.h>

#define WEXITSTATUS(stat_val) 0
#define WIFEXITED(stat_val) 0
#define WIFSIGNALED(stat_val) 0
#define WTERMSIG(stat_val) 0

enum wait_options {
        WNOHANG = 1,
};

int waitpid(pid_t pid, int *status, enum wait_options options);

#endif // _SYS_WAIT_H_

