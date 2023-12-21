#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void fail_unless(int a)
{
    if (a < 0)
        exit(1);
}

int exec(const char *stdio_file, char **argv)
{
    pid_t child;

    child = fork();
    if (child == -1) {
        perror("fork()");
        return -1;
    }
    if (child == 0) {
        if (stdio_file) {
            close(0);
            close(1);
            close(2);
            fail_unless(open(stdio_file, O_RDONLY));
            fail_unless(open(stdio_file, O_WRONLY));
            fail_unless(open(stdio_file, O_WRONLY));
        }

        printf("Welcome to Nightingale\n");
        execve(argv[0], argv, NULL);

        printf("init failed to run sh\n");
        perror("execve");
        exit(127);
    }
    return child;
}

int wait_for(pid_t pid)
{
    int return_code;
    while (true) {
        int fpid = waitpid(pid, &return_code, 0);
        if (fpid < 0 && errno == EINTR)
            continue;
        if (fpid < 0 && errno == ECHILD)
            break;
        if (fpid < 0) {
            perror("waitpid");
            exit(1);
        }
        break;
    }
    return return_code;
}

_Noreturn void run_sh_forever(const char *stdio_file)
{
    while (true) {
        int child = exec(stdio_file, (char *[]) { "/bin/sh", NULL });
        wait_for(child);
    }
}

void cleanup_children(int arg)
{
    (void)arg;
    int status;
    pid_t pid = 0;
    while (pid != -1) {
        pid = waitpid(-1, &status, 0);
        // fprintf(stderr, "init reaped %i\n", pid);
    }
}

int main()
{
    fail_unless(mkdir("/dev", 0755));
    fail_unless(mknod("/dev/null", 0666, 0));
    fail_unless(mknod("/dev/zero", 0666, 1));
    fail_unless(mknod("/dev/random", 0666, 2));
    fail_unless(mknod("/dev/inc", 0666, 3));
    fail_unless(mknod("/dev/serial", 0666, 1 << 16));
    fail_unless(mknod("/dev/serial2", 0666, 1 << 16 | 1));

    fail_unless(mkdir("/proc", 0755));
    fail_unless(mount("/proc", _FS_PROCFS, "proc"));

    fail_unless(open("/dev/serial", O_RDONLY));
    fail_unless(open("/dev/serial", O_WRONLY));
    fail_unless(open("/dev/serial", O_WRONLY));

    int tester = exec("/dev/null", (char *[]) { "/bin/test", NULL });
    int return_code = wait_for(tester);
    if (return_code != 0) {
        printf("WARNING: test failed\n");
    }

    signal(SIGCHLD, cleanup_children);

    // if (fork())
    //     run_sh_forever("/dev/serial2");
    run_sh_forever(NULL);
    assert(0);
}
