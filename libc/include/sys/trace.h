#pragma once
#ifndef _SYS_TRACE_H_
#define _SYS_TRACE_H_

#include <ng/trace.h>
#include <sys/types.h>

int trace(enum trace_command cmd, pid_t pid, void *addr, void *data);

#endif // _SYS_TRACE_H_
