
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <nightingale.h>

#define FLAG "SUCCESS"

#define ARRAY_LEN(a) ((sizeof(a) / sizeof(*a)))

void simple() {
        printf("%s\n", FLAG);
}

void math() {
        int x = 10;
        int y = 100;
        int z = 1000;

        if (x + y + z != 1110) {
                return;
        }

        x = 100;
        y = 100;

        if (x * y != 10000) {
                return;
        }

        x = 1000;
        y = 10;

        if (x / y != 100) {
                return;
        }

        printf("%s\n", FLAG);
}

void float_math() {
        double x = 10.0;
        double y = 100.0;
        double z = 1000.0;

        if (x + y + z != 1110.0) {
                return;
        }

        x = 100.0;
        y = 100.0;

        if (x * y != 10000.0) {
                return;
        }

        x = 1000.0;
        y = 10.0;

        if (x / y != 100.0) {
                return;
        }

        x = 0.5;
        y = 0.5;

        if (x / y != 1.0) {
                return;
        }

        printf("%s\n", FLAG);
}

void subprocess() {
        pid_t pid = fork();

        int status = 0xaaaa;
        if (pid) {
                waitpid(pid, &status, 0);
        } else {
                execve("/bin/echo", (char *[]){"echo", "test", "foobar", NULL}, NULL);
                __builtin_unreachable();
        }

        if (status != 0) {
                return;
        }

        printf("%s\n", FLAG);
}

void file() {
        char *filename = "/bin/text_file";

        int fd = open(filename, O_RDONLY);

        if (fd < 3) {
                return;
        }

        char buffer[1024] = {0};

        int nbytes = read(fd, buffer, 1024);

        if (nbytes < 0) {
                return;
        }

        if (strcmp(buffer, "This is a test file\n") != 0) {
                return;
        }

        printf("%s\n", FLAG);
}


struct test_case {
        char *name;
        void (*fn)();
};

struct test_case tests[] = {
        { "test_tests_work", simple },
        { "test_math", math },
        { "test_float_math", float_math },
        { "test_subprocess", subprocess },
        { "test_files", file },
//        { "test_fstdio",
//        { "test_uname",
//        { "test_mmap",
};

int main(int argc, char **argv) {
        if (argc != 2) {
                printf("a test to run is required\n");
                exit(EXIT_FAILURE);
        }

        if (strcmp(argv[1], "all") == 0) {
                // run all the tests
                for (int i=0; i<ARRAY_LEN(tests); i++) {
                        tests[i].fn();
                }
                exit(EXIT_SUCCESS);
        }

        for (int i=0; i<ARRAY_LEN(tests); i++) {
                // run a specific test
                if (strcmp(argv[1], tests[i].name) == 0) {
                        tests[i].fn();

                        // these are run by test_runner, so exit the VM now
                        haltvm();
                }
        }
        printf("test %s not found\n", argv[1]);
        exit(EXIT_FAILURE);
}

