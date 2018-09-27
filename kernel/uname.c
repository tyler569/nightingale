
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
#if X86_64
    strcpy((char*)&n->machine, "x86_64");
#elif I686
    strcpy((char*)&n->machine, "i686");
#endif
    RETURN_VALUE(0);
}

