#include <assert.h>
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
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>

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
            int fd;
            fd = open(stdio_file, O_RDONLY);
            dup2(fd, 0);
            close(fd);
            fd = open(stdio_file, O_WRONLY);
            dup2(fd, 1);
            close(fd);
            fd = open(stdio_file, O_WRONLY);
            dup2(fd, 2);
            close(fd);
        }

        printf("Welcome to Nightingale\n");
        execve(argv[0], argv, NULL);

        printf("init failed to run sh\n");
        perror("execve");
        exit(127);
    }
    return child;
}

void run_sh_forever(const char *device)
{
    while (true) {
        int child = exec(device, (char *[]) { "/bin/sh", NULL });
        int return_code;
        while (true) {
            int pid = waitpid(child, &return_code, 0);
            if (pid < 0 && errno == EINTR)
                continue;
            if (pid < 0) {
                perror("waitpid");
                exit(1);
            }
            break;
        }
        if (return_code)
            assert("init failed to start the shell" && 0);
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

void fail_unless(int a)
{
    if (a < 0)
        exit(1);
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

    signal(SIGCHLD, cleanup_children);

    if (fork())
        run_sh_forever("/dev/serial2");
    run_sh_forever(NULL);
    assert(0);
}
