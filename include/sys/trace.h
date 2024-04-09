#pragma once

#include <ng/cpu.h>
#include <ng/trace.h>
#include <sys/cdefs.h>
#include <sys/types.h>

BEGIN_DECLS

int trace(enum trace_command cmd, pid_t pid, void *addr, void *data);

END_DECLS
