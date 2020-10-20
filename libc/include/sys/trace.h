#pragma once
#ifndef _SYS_TRACE_H_
#define _SYS_TRACE_H_

#include <sys/types.h>
#include <ng/trace.h>

int trace(enum trace_command cmd, pid_t pid, void *addr, void *data);

#endif // _SYS_TRACE_H_
