
#pragma once
#ifndef _SYS_TRACE_H_
#define _SYS_TRACE_H_

#include <sys/types.h>
#include <ng/trace.h>

int trace(pid_t pid, enum trace_command cmd, void *addr, void *data);

#endif // _SYS_TRACE_H_
