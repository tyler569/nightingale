
#include <stdio.h>
#include <sys/utsname.h>

int main() {
        struct utsname buf;
        uname(&buf);
        printf("%s %s %s\n", buf.sysname, buf.release, buf.machine);
        return 0;
}
