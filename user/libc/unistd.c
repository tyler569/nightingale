
#include "unistd.h"
#include <errno.h>
#include <ng/syscall_consts.h>
#include <poll.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdnoreturn.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include "syscall.h"

/* end syscall stubs */

int isatty(int fd) {
        // does not account for piped stdins.
        return fd < 3;
}

