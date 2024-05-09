#pragma once

#include <sys/cdefs.h>
#include <sys/syscall_consts.h>
#include <sys/types.h>

BEGIN_DECLS

struct submission {
	enum ng_syscall syscall_num;
	uintptr_t args[6];
};

END_DECLS
