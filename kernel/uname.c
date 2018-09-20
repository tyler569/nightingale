
#include <basic.h>
#include <string.h>
#include <syscall.h>
#include "uname.h"

struct syscall_ret sys_uname(struct utsname* n) {
    if (!n)
        RETURN_ERROR(EINVAL);
    memset(n, 0, sizeof(struct utsname));
    strcpy((char*)&n->sysname, "nightingale");
    strcpy((char*)&n->nodename, "");
    strcpy((char*)&n->release, NIGHTINGALE_VERSION);
    strcpy((char*)&n->version, "");
#if defined(__x86_64__)
    strcpy((char*)&n->machine, "x86_64");
#elif defined(__i686__)
    strcpy((char*)&n->machine, "i686");
#else
#error "unsupported machine at uname"
#endif
    RETURN_VALUE(0);
}

