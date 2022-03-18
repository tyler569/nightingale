#include <stdio.h>
#include <stdlib.h>
#include <sys/utsname.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    struct utsname buf;
    uname(&buf);

    enum {
        NONE = 0,
        SYSNAME = (1 << 0),
        NODENAME = (1 << 1),
        RELEASE = (1 << 2),
        VERSION = (1 << 3),
        MACHINE = (1 << 4),
    };
    int prints = NONE;

    int c;
    while ((c = getopt(argc, argv, "asnrvmpioh")) != -1) {
        switch (c) {
        case 'a':
            prints |= 0xFFFF;
            break;
        case 's':
            prints |= SYSNAME;
            break;
        case 'n':
            prints |= NODENAME;
            break;
        case 'r':
            prints |= RELEASE;
            break;
        case 'v':
            prints |= VERSION;
            break;
        case 'm':
            prints |= MACHINE;
            break;
        case '?':
            printf("Help todo\n");
            exit(EXIT_FAILURE);
        }
    }

    if (prints == NONE)
        prints = SYSNAME;

    if (prints & SYSNAME)
        printf("%s ", buf.sysname);
    if (prints & NODENAME)
        printf("%s ", buf.nodename);
    if (prints & RELEASE)
        printf("%s ", buf.release);
    if (prints & VERSION)
        printf("%s ", buf.version);
    if (prints & MACHINE)
        printf("%s ", buf.machine);
    printf("\n");

    return 0;
}
