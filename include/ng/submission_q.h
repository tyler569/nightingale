#pragma once

#include <ng/syscall_consts.h>
#include <sys/types.h>

struct submission {
    enum ng_syscall syscall_num;
    uintptr_t args[6];
};
