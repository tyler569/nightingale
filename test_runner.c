
#define _GNU_SOURCE
#include <ng/string.h>
#include <errno.h>
#include <sched.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

int max(int a, int b) { return a > b ? a : b; }

int timeout_thread(int seconds) {
        int timeout_pipe[2];
        pipe(timeout_pipe);

        if (!fork()) {
                // TODO: fork is pretty heavyweight for this
                // I imagine a thread (clone(2)) would be better
                close(timeout_pipe[0]);
                sleep(seconds);
                write(timeout_pipe[1], "x", 1);
                exit(0);
        } else {
                close(timeout_pipe[1]);
                return timeout_pipe[0];
        }
}

int run_nightingale(const char *test_name, const char *flag, int timeout_fd) {
        int input[2], output[2];
        pipe(input);
        pipe(output);

        pid_t child = fork();

        if (!child) {
                close(input[1]);
                close(output[0]);
                close(0);
                close(1);
                dup2(input[0], 0);
                dup2(output[1], 1);
                setsid();
                execvp("ruby", (char *[]){"ruby", "run.rb", NULL});
        } else {
                close(input[0]);
                close(output[1]);
                write(input[1], test_name, strlen(test_name));
                write(input[1], "\n", 1);
        }

#define MAX (1 << 16) // 64k

        int fd = output[0];
        int timeout = timeout_fd;

        char *buf = calloc(1, MAX);
        int cnt = 0;
        int res = 1;
        fd_set fds_;
        fd_set *fds = &fds_;

        while (res) {
                FD_ZERO(fds);
                FD_SET(fd, fds);
                FD_SET(timeout, fds);

                select(max(fd, timeout) + 1, fds, NULL, NULL, NULL);

                if (FD_ISSET(fd, fds)) {
                        res = read(fd, buf + cnt, MAX - cnt);
                        cnt += res;
                }
                if (FD_ISSET(timeout, fds)) {
                        break;
                }
        }

        kill(-child, 2); // SIGINT

        close(fd);
        close(timeout);

        char *success = strstr(buf, flag);
        int result = success ? 1 : 0;

        // printf("%s\n", buf);
        free(buf);
        return result;
}

int run_test(const char *test_name, const char *test_cmd, const char *flag) {
        int timeout = timeout_thread(2);
        int res = run_nightingale(test_cmd, flag, timeout);
        printf("%s:'%s': %s\n", test_name, test_cmd, res ? "PASS" : "FAIL");
        return res;
}

typedef struct test_desc {
        const char *name;
        const char *cmd;
        const char *flag;
} test_desc;

test_desc tests_to_run[] = {
    {"test tests", "echo SUCCESS", "SUCCESS"},
    {"test tests", "echo FAILURE", "SUCCESS"},
};

int run_all_tests() {
        int nr_tests = sizeof(tests_to_run) / sizeof(*tests_to_run);
        int total_failures = 0;
        for (int i = 0; i < nr_tests; i++) {
                test_desc test = tests_to_run[i];
                int res = run_test(test.name, test.cmd, test.flag);
                if (res == 0)
                        total_failures += 1;
        }
        return total_failures;
}

int main() {
        int failures = run_all_tests();
        printf("%i failures\n", failures);

        return failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}