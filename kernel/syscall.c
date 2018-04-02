
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <basic.h>
#include <print.h>
#include <fs/vfs.h>
#include "syscall.h"
// #include <sys/syscall.h> // TODO: sysroot include with things like syscall numbers
// #include <sys/error.h> // TODO: sysroot include with errors

#include <syscalls.h> // syscall sys_* prototypes

#define SYS_INVALID 0
#define SYS_DEBUGPRINT 1
#define SYS_EXIT 2

// TODO:
#define SYS_OPEN 3
#define SYS_READ 4
#define SYS_WRITE 5

#define SUCCESS 0
#define ERR_NOSUCH 1

// Extra arguments are not passed or clobbered in registers, that is
// handled in arch/, anything unused is ignored here.
// arch/ code also handles the multiple return
struct syscall_ret do_syscall(int syscall_num,
        uintptr_t arg1, uintptr_t arg2, uintptr_t arg3,
        uintptr_t arg4, uintptr_t arg5, uintptr_t arg6) {

    // printf("syscall: %i\n", syscall_num);

    struct syscall_ret ret;

    switch (syscall_num) {
    case SYS_DEBUGPRINT:
        ;
        size_t printed = printf("%s", (void *)arg1);
        ret.value = printed;
        ret.error = SUCCESS;
        break;
    case SYS_EXIT:
        sys_exit(arg1);
        ret.value = 0;
        ret.error = SUCCESS;
        break;
    case SYS_READ:
        return sys_read(arg1, arg2, arg3);
        break;
    case SYS_WRITE:
        return sys_write(arg1, arg2, arg3);
        break;
    default:
        printf("syscall: %i does not exist\n", syscall_num);
        ret.error = ERR_NOSUCH;
    }

    return ret;
}

