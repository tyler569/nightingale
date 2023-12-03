#include <nightingale.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <unistd.h>

noreturn void help(const char *progname)
{
    fprintf(stderr,
        "%s: usage %s [-dhf] [-p PID] [program [arguments...]]\n"
        "  -h            Show this help\n"
        "  -d            Disable syscall trace\n"
        "  -f            Follow children\n"
        "  -p PID        Set syscall state for PID, instead of execing\n",
        progname, progname);
    exit(0);
}

int main(int argc, char **argv)
{
    if (!argv[1]) {
        fprintf(stderr, "No command specified\n");
        exit(EXIT_FAILURE);
    }

    int c;
    pid_t pid = 0;
    int state = 1;
    while ((c = getopt(argc, argv, "dhfp:")) != -1) {
        switch (c) {
        case 'd':
            state = 0;
            break;
        case 'f':
            state |= 2;
            break;
        case 'p':
            pid = atoi(optarg);
            break;
        case 'h':
        case '?':
            help(argv[0]);
        }
    }

    syscall_trace(pid, state);

    if (pid == 0)
        execve(argv[optind], argv + optind, NULL);
    return 0;
}
