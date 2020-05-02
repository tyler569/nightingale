
#include <basic.h>
#include <ng/syscall.h>
#include <ng/trace.h>
#include <nc/errno.h>

sysret sys_trace(pid_t pid, enum trace_command cmd, void *addr, void *data) {
        return -EINVAL;
}
