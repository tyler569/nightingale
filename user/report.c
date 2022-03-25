#include <ng/event_log.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

const char *log_names[] = {
    [EVENT_ALLOC] = "alloc",
    [EVENT_FREE] = "free",
    [EVENT_THREAD_NEW] = "thnew",
    [EVENT_THREAD_SWITCH] = "thsw",
    [EVENT_THREAD_DIE] = "thdie",
    [EVENT_THREAD_REAP] = "threap",
    [EVENT_THREAD_ENQUEUE] = "thenq",
    [EVENT_SYSCALL] = "syscall",
    [EVENT_SIGNAL] = "signal",
};

extern int __ng_report_events(long event_mask);

int log_type(const char *string)
{
    if (strcmp(string, "--") == 0)
        return -2;
    for (size_t i = 0; i < ARRAY_LEN(log_names); i++) {
        if (log_names[i] && strcmp(string, log_names[i]) == 0)
            return i;
    }
    return -1;
}

int main(int argc, char **argv)
{
    long mask = 0;
    argv = argv + 1;

    if (argc < 2) {
        printf("Available reports:\n");
        for (size_t i = 0; i < ARRAY_LEN(log_names); i++)
            printf(" - %s\n", log_names[i]);
        return 0;
    }

    for (char **arg = argv; *arg; arg++) {
        int l = log_type(*arg);
        if (l == -2) {
            argv = arg + 1;
            break;
        }
        if (l == -1) {
            argv = arg;
            break;
        }
        mask |= (1l << l);
    }

    __ng_report_events(mask);
    execvp(argv[0], argv);
}
