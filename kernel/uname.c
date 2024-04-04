#include <errno.h>
#include <ng/string.h>
#include <ng/syscall.h>
#include <sys/utsname.h>
#include <version.h>

#ifdef __x86_64__
#define UNAME_ARCH "x86_64"
#endif

sysret sys_uname(struct utsname *n)
{
    if (!n)
        return -EINVAL;
    memset(n, 0, sizeof(struct utsname));
    strcpy((char *)&n->sysname, "nightingale");
    strcpy((char *)&n->nodename, "ng");
    strcpy((char *)&n->release, NIGHTINGALE_VERSION);
    strcpy((char *)&n->version, "v");
    strcpy((char *)&n->machine, UNAME_ARCH);
    return 0;
}
