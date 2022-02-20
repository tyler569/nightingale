#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

bool go_slow = true;

void slow() {
    for (int i = 0; i < 1000000; i++)
        asm ("");
}

void print_my_letter(char c) {
    for (int j = 0; j < 10; j++) {
        if (go_slow)
            slow();

        fprintf(stderr, "%c", c);
    }
    exit(0);
}

void help(const char *progname) {
    fprintf(
        stderr,
        "%s: usage threads [-hfw] [-n THREADS]\n"
        "  -h            Show this help\n"
        "  -f            Don't slow prints\n"
        "  -n THREADS    Start THREADS processes\n"
        "  -w            Wait between each fork\n",
        progname
    );
}

int main(int argc, char **argv) {
    int pid = getpid();
    int child;
    int nthreads = 26;
    bool wait_each = false;
    char c, l = 'A';

    while ((c = getopt(argc, argv, "hfn:w")) != -1) {
        switch (c) {
        case 'f':
            go_slow = false;
            break;
        case 'n':
            nthreads = strtol(optarg, NULL, 10);
            break;
        case 'w':
            wait_each = true;
            break;
        case 'h':         // FALLTHROUGH
        case '?':
            help(argv[0]);
            return 0;
        }
    }

    setpgid(pid, pid);

    for (int i = 0; i < nthreads; i++) {
        if (!(child = fork()))
            print_my_letter(l);
        if (wait_each)
            waitpid(child, &child, 0);
        l += 1;
        if (l == 'Z')
            l = 'A';
    }

    while (errno != ECHILD) {
        // collect all the zombies

        int status = 0;
        waitpid(-pid, &status, 0);
    }

    printf("\n");

    return 0;
}
