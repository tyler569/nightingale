#include <basic.h>
#include <ng/event_log.h>
#include <ng/panic.h>
#include <ng/syscall.h>
#include <ng/syscall_consts.h>
#include <ng/syscalls.h> // syscall sys_* prototypes
#include <ng/thread.h>
#include <ng/vmm.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include "autogenerated_syscalls_kernel.c"

int syscall_call_counts[SYSCALL_TABLE_SIZE];

void syscall_entry(int syscall) {
    syscall_call_counts[syscall]++;
    if (running_thread->tracer) {
        trace_syscall_entry(running_thread, syscall);
    }
}

void syscall_exit(int syscall) {
    if (running_thread->tracer)
        trace_syscall_exit(running_thread, syscall);
}

int syscall_register(
    int number,
    const char *name,
    sysret (*fn)(),
    const char *debug,
    unsigned ptr_mask
) {
    if (number < 0 || number >= SYSCALL_TABLE_SIZE)
        return -1;
    if (syscall_table[number])
        return -1;
    syscall_table[number] = fn;
    syscall_names[number] = name;
    syscall_debuginfos[number] = debug;
    syscall_ptr_mask[number] = ptr_mask;
    return 0;
}

enum ptr_status {
    PTR_GOOD = 0,
    PTR_BAD = 1,
};

enum ptr_status syscall_check_pointer(uintptr_t ptr) {
    if (ptr == 0)
        return PTR_GOOD;
    uintptr_t *pte_ptr = vmm_pte_ptr(ptr);
    if (!pte_ptr)
        return PTR_BAD;
    if (!(*pte_ptr & PAGE_USERMODE))
        return PTR_BAD;
    return PTR_GOOD;
}

enum ptr_status check_ptr(unsigned enable, uintptr_t ptr) {
    if (enable) {
        return syscall_check_pointer(ptr);
    } else {
        return PTR_GOOD;
    }
}

// Extra arguments are not passed or clobbered in registers, that is
// handled in arch/, anything unused is ignored here.
// arch/ code also handles the multiple return
sysret do_syscall(interrupt_frame *frame) {
    sysret ret;
    enum ng_syscall syscall_num = FRAME_SYSCALL(frame);
    syscall_entry(syscall_num);

    uintptr_t arg1 = FRAME_ARG1(frame);
    uintptr_t arg2 = FRAME_ARG2(frame);
    uintptr_t arg3 = FRAME_ARG3(frame);
    uintptr_t arg4 = FRAME_ARG4(frame);
    uintptr_t arg5 = FRAME_ARG5(frame);
    uintptr_t arg6 = FRAME_ARG6(frame);

    if (syscall_num >= SYSCALL_TABLE_SIZE || syscall_num <= 0) {
        return -ENOSYS;
    }
    sysret (*syscall)() = syscall_table[syscall_num];
    if (!syscall)
        return -ENOSYS;

    if (running_thread->flags & TF_SYSCALL_TRACE) {
        printf("[%i:%i] ", running_process->pid, running_thread->tid);
        const char *info = syscall_debuginfos[syscall_num];
        printf(info, arg1, arg2, arg3, arg4, arg5, arg6);
    }

    unsigned mask = syscall_ptr_mask[syscall_num];
    if (
        check_ptr(mask & 0x01, arg1) || check_ptr(mask & 0x02, arg2) ||
        check_ptr(mask & 0x04, arg3) || check_ptr(mask & 0x08, arg4) ||
        check_ptr(mask & 0x10, arg5) || check_ptr(mask & 0x20, arg6)
    ) {
        ret = -EFAULT;
        goto out;
    }

    log_event(
        EVENT_SYSCALL,
        "syscall: thread_id %i: %i\n",
        running_thread->tid,
        syscall_num,
        arg1,
        arg2,
        arg3,
        arg4,
        arg5,
        arg6
    );

    if (
        syscall_num == NG_EXECVE || syscall_num == NG_FORK ||
        syscall_num == NG_CLONE0
    ) {
        ret = syscall(frame, arg1, arg2, arg3, arg4, arg5, arg6);
    } else {
        ret = syscall(arg1, arg2, arg3, arg4, arg5, arg6);
    }

out:
    if (running_thread->flags & TF_SYSCALL_TRACE) {
        if (syscall_num == NG_SYSCALL_TRACE) {
            // This is just here to mark this as a strace return,
            // since it can be confusing that " -> 0" appears
            // after some other random syscall when the strace
            // call returns.
            printf("XX");
        }
        if (ret >= 0 && ret < 0x100000) {
            printf(" -> %lu\n", ret);
        } else if (ret >= 0 || ret < -0x1000) {
            printf(" -> %#lx\n", ret);
        } else {
            printf(" -> <%s>\n", errno_names[-ret]);
        }
    }

    FRAME_RETURN(frame) = ret;
    syscall_exit(syscall_num);
    handle_pending_signals();

    return ret;
}

void proc_syscalls(struct open_file *ofd) {
    proc_sprintf(ofd, "num name addr calls\n");
    for (int i = 0; i < SYSCALL_TABLE_SIZE; i++) {
        const char *name = syscall_names[i] ? syscall_names[i] : "-";
        if (i >= SYSCALL_MAX && !syscall_table[i])
            continue;
        proc_sprintf(
            ofd,
            "%3i %15s %#018jx %i\n",
            i,
            name,
            (uintptr_t)syscall_table[i],
            syscall_call_counts[i]
        );
    }
}
