
#include <basic.h>
#include <ng/string.h>
#include <ng/syscall.h>
#include <ng/uname.h>
#include <errno.h>

#if X86_64
#define UNAME_ARCH "x86_64"
#endif

sysret sys_uname(struct utsname *n) {
        if (!n)
                return -EINVAL;
        memset(n, 0, sizeof(struct utsname));
        strcpy((char *)&n->sysname, "nightingale");
        strcpy((char *)&n->nodename, "");
        strcpy((char *)&n->release, NIGHTINGALE_VERSION);
        strcpy((char *)&n->version, "");
        strcpy((char *)&n->machine, UNAME_ARCH);
        return 0;
}
