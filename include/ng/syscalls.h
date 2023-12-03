#pragma once
#ifndef NG_SYSCALLS_H
#define NG_SYSCALLS_H

#include <ng/cpu.h>
#include <ng/fs.h>
#include <ng/syscall.h>
#include <ng/thread.h>
#include <ng/trace.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/cdefs.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/ttyctl.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <dirent.h>
#include <poll.h>
#include <signal.h>
#include <syscall_types.h>
#include <time.h>

BEGIN_DECLS

#include "autogenerated_syscalls_kernel.h"

END_DECLS

#endif // NG_SYSCALLS_H
