
#include <stddef.h>
#include <stdint.h>
#include <string.h>
//#include <basic.h>
#include <print.h>
#include <thread.h>
#include <fs/vfs.h>
#include <arch/x86/cpu.h>
#include "syscall.h"

#include <syscalls.h> // syscall sys_* prototypes

#include <ng_syscall.h> // will this stay here?

// TODO: use this table
void *syscalls[] = {
    [SYS_DEBUGPRINT] = NULL, // deprecated
    [SYS_EXIT] = sys_exit,
    [SYS_OPEN] = NULL,       // unimplemented
    [SYS_READ] = sys_read,
    [SYS_WRITE] = sys_write,
    [SYS_FORK] = sys_fork,
    [SYS_TOP] = sys_top,
    [SYS_GETPID] = sys_getpid,
    [SYS_GETTID] = sys_gettid,
    [SYS_EXECVE] = sys_execve,
    [SYS_WAIT4] = sys_wait4,
    [SYS_SOCKET] = sys_socket,
    [SYS_BIND0] = sys_bind0,
    [SYS_CONNECT0] = sys_connect0,
    [SYS_STRACE] = sys_strace,
};

// Extra arguments are not passed or clobbered in registers, that is
// handled in arch/, anything unused is ignored here.
// arch/ code also handles the multiple return
struct syscall_ret do_syscall(int syscall_num,
        uintptr_t arg1, uintptr_t arg2, uintptr_t arg3,
        uintptr_t arg4, uintptr_t arg5, uintptr_t arg6,
        interrupt_frame *frame) {

    // printf("syscall: %i\n", syscall_num);
    
    //uintptr_t rsp;
    //asm volatile ("mov %%rsp, %0" : "=r"(rsp));
    //printf("syscall stack: %lx\n", rsp);
    
    // print_registers(frame);

    struct syscall_ret ret;

    // TODO: strace "String str ..." format

    switch (syscall_num) {
    case SYS_DEBUGPRINT:
        ;
        if (running_thread->strace) {
            printf("debugprint(%lx)", arg1);
        }

        size_t printed = printf("%s", (void *)arg1);
        ret.value = printed;
        ret.error = SUCCESS;

        if (running_thread->strace) {
            printf(" -> { value = %lx, error = %lx };\n", ret.value, ret.error);
        }
        break;
    case SYS_EXIT:
        if (running_thread->strace) {
            printf("exit(%lx)", arg1);
        }

        sys_exit(arg1);
        ret.value = 0;
        ret.error = SUCCESS;

        if (running_thread->strace) {
            printf(" -> { value = %lx, error = %lx };\n", ret.value, ret.error);
        }
        break;
    case SYS_READ:
        if (running_thread->strace) {
            printf("read(%lx, %lx, %lx)", arg1, arg2, arg3);
        }

        ret = sys_read(arg1, (void *)arg2, arg3);

        if (running_thread->strace) {
            printf(" -> { value = %lx, error = %lx };\n", ret.value, ret.error);
        }
        return ret;
        break;
    case SYS_WRITE:
        if (running_thread->strace) {
            printf("write(%lx, %lx, %lx)", arg1, arg2, arg3);
        }

        ret = sys_write(arg1, (void *)arg2, arg3);

        if (running_thread->strace) {
            printf(" -> { value = %lx, error = %lx };\n", ret.value, ret.error);
        }
        return ret;
        break;
    case SYS_FORK:
        if (running_thread->strace) {
            printf("fork()");
        }

        ret = sys_fork(frame);

        if (running_thread->strace) {
            printf(" -> { value = %lx, error = %lx };\n", ret.value, ret.error);
        }
        return ret;
        break;
    case SYS_TOP:
        if (running_thread->strace) {
            printf("top()");
        }

        ret = sys_top();

        if (running_thread->strace) {
            printf(" -> { value = %lx, error = %lx };\n", ret.value, ret.error);
        }
        return ret;
        break;
    case SYS_GETPID:
        if (running_thread->strace) {
            printf("getpid()");
        }

        ret = sys_getpid();

        if (running_thread->strace) {
            printf(" -> { value = %lx, error = %lx };\n", ret.value, ret.error);
        }
        return ret;
        break;
    case SYS_GETTID:
        if (running_thread->strace) {
            printf("gettid()");
        }

        ret = sys_gettid();

        if (running_thread->strace) {
            printf(" -> { value = %lx, error = %lx };\n", ret.value, ret.error);
        }
        return ret;
        break;
    case SYS_EXECVE:
        if (running_thread->strace) {
#if 0 // someday strace will pretty print like this
      // I'll definitely need some safety first
      // maybe a print_string_array helper that can handle NULL
            printf("execve(\"%s\", [", arg1);

            char **argv = (void *)arg2;
            if (argv) {
                for (int i=0; i<32; i++) {
                    if (argv[i])
                        printf("\"%s\"", argv[1]);
                    if (argv[i + 1])
                        printf(", ");
                }
            }
            printf("], %#lx)\n", arg3);
#endif
            printf("execve(%#lx, %#lx, %#lx)", arg1, arg2, arg3);
        }


        ret = sys_execve(frame, (void *)arg1, (void *)arg2, (void *)arg3);

        if (running_thread->strace) {
            printf(" -> { value = %lx, error = %lx };\n", ret.value, ret.error);
        }
        return ret;
        break;
    case SYS_WAIT4:
        if (running_thread->strace) {
            printf("wait4(%i)", arg1);
        }

        ret = sys_wait4(arg1);

        if (running_thread->strace) {
            printf(" -> { value = %lx, error = %lx };\n", ret.value, ret.error);
        }
        return ret;
        break;
    case SYS_SOCKET:
        if (running_thread->strace) {
            printf("socket(%i, %i, %i)", arg1, arg2, arg3);
        }

        ret = sys_socket(arg1, arg2, arg3);

        if (running_thread->strace) {
            printf(" -> { value = %lx, error = %lx };\n", ret.value, ret.error);
        }
        return ret;
        break;
    case SYS_BIND0:
        if (running_thread->strace) {
            printf("bind0(%i, %#x, %lu)", arg1, arg2, arg3);
        }

        ret = sys_bind0(arg1, arg2, arg3);

        if (running_thread->strace) {
            printf(" -> { value = %lx, error = %lx };\n", ret.value, ret.error);
        }
        return ret;
        break;
    case SYS_CONNECT0:
        if (running_thread->strace) {
            printf("connect0(%i, %#x, %hu)", arg1, arg2, arg3);
        }

        ret = sys_connect0(arg1, arg2, arg3);

        if (running_thread->strace) {
            printf(" -> { value = %lx, error = %lx };\n", ret.value, ret.error);
        }
        return ret;
        break;
    case SYS_STRACE:
        ret = sys_strace(arg1);

        // explicitly after since this function toggles strace printing
        if (running_thread->strace) {
            printf("strace(%s)", arg1 ? "true" : "false");
        }

        if (running_thread->strace) {
            printf(" -> { value = %lx, error = %lx };\n", ret.value, ret.error);
        }
        return ret;
        break;
    default:
        printf("syscall: %i does not exist\n", syscall_num);
        ret.error = EINVAL;
    }

    return ret;
}

