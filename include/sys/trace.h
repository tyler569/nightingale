#pragma once
#ifndef _SYS_TRACE_H_
#define _SYS_TRACE_H_

#include "ng/cpu.h"
#include "ng/trace.h"
#include <sys/cdefs.h>
#include <sys/types.h>

BEGIN_DECLS

int trace(enum trace_command cmd, pid_t pid, void *addr, void *data);

END_DECLS

#endif // _SYS_TRACE_H_
