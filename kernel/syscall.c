
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <basic.h>
#include <print.h>
#include <thread.h>
#include <fs/vfs.h>
#include <arch/x86/cpu.h>
#include "syscall.h"
// #include <sys/syscall.h> // TODO: sysroot include with things like syscall numbers
// #include <sys/error.h> // TODO: sysroot include with errors

#include <syscalls.h> // syscall sys_* prototypes

#define SYS_INVALID 0
#define SYS_DEBUGPRINT 1
#define SYS_EXIT 2

#define SYS_OPEN 3 // TODO
#define SYS_READ 4
#define SYS_WRITE 5

#define SYS_FORK 6
#define SYS_TOP 7

#define SYS_GETPID 8
#define SYS_GETTID 9

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
    default:
        printf("syscall: %i does not exist\n", syscall_num);
        ret.error = ERR_NOSUCH;
    }

    return ret;
}

