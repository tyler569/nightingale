#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int tests_failed = 0;
int tests_ran = 0;

int test(char *str, int assertion) {
    tests_ran += 1;

    if (assertion) {
        printf("passed : '%s'\n", str);
    } else {
        printf("FAILED : '%s'\n", str);
        tests_failed += 1;
    }

    return assertion;
}

int test_equal(char *str, int v1, int v2) {
    tests_ran += 1;

    if (v1 == v2) {
        printf("passed : '%s' (%i == %i)\n", str, v1, v2);
    } else {
        printf("FAILED : '%s' (%i != %i)\n", str, v1, v2);
        tests_failed += 1;
    }

    return v1 == v2;
}

int test_str_equal(const char *a, const char *b) {
    tests_ran += 1;

    if (strcmp(a, b) == 0) {
        printf("passed : \"%s\" == \"%s\"\n", a, b);
    } else {
        printf("FAILED : \"%s\" == \"%s\"\n", a, b);
        tests_failed += 1;
    }

    return strcmp(a, b);
}

sig_atomic_t signal_test_value = 0;

void sigusr1(int signal) {
    signal_test_value += 1;
}

#define TEST(x) test(#x, x)
#define TEST_EQ(x, y) test_equal(#x, x, y)
#define TEST_STR_EQ(a, b) test_str_equal(a, b)

void test_sprintf(const char *expect, const char *format, ...) {
    char buf[256] = {0};
    int fail = 0;

    va_list args;
    va_start(args, format);

    int len = vsprintf(buf, format, args);

    if (strcmp(buf, expect) != 0) {
        printf(
            "FAILED : sprintf(\"%s\", ...) != \"%s\"\n",
            format,
            expect
        );
        fail = 1;
        tests_failed += 1;
    }
    if (strlen(expect) != len) {
        printf(
            "FAILED : sprintf(\"%s\", ...) returned wrong strlen\n",
            format
        );
        fail = 1;
        tests_failed += 1;
    }
    if (fail == 0) {
        printf(
            "passed : sprintf(\"%s\", ...) == \"%s\"\n",
            format,
            expect
        );
    }
}

int compare_ints(const void *a, const void *b) {
    return *(int *)a > *(int *)b ? 1 : -1;
}

void test_qsort() {
    int numbers[64];
    FILE *random = fopen("/dev/random", "r");
    fread(numbers, 4, 64, random);
    qsort(numbers, 64, 4, compare_ints);
    for (int i = 0; i < 63; i++) {
        if (numbers[i] > numbers[i + 1]) {
            printf("FAIL : qsort is not working\n");
            return;
        }
    }
    printf("passed : qsort is working\n");
}

int main() {
    // real basics
    TEST(1 == 1);
    TEST(2 + 2 == 4);
    TEST_EQ(1 + 3, 4);

    // stdlib stuff
    TEST(strcmp("foo", "bar") != 0);

    test_sprintf("test", "test");
    test_sprintf("test 100", "test %i", 100);
    test_sprintf("test 0x100", "test %#x", 0x100);
    test_sprintf("     a", "%6s", "a");
    test_sprintf("1 2 3 4 5", "%i %d %o %li %u", 1, 2, 3, 4l, 5u);
    test_sprintf("0x001234", "%#08x", 0x1234);
    test_sprintf("Hello", "%.*s", 5, "Hello World", 5);

    TEST_STR_EQ("Hello", strdup("Hello"));
    TEST_STR_EQ("Hell", strndup("Hello", 4));

    // syscall stuff
    TEST(getpid() > 0);
    TEST_EQ(errno, SUCCESS);

    signal(SIGUSR1, sigusr1);
    raise(SIGUSR1);
    TEST(signal_test_value == 1);

    test_qsort();

    // other programs
    // mkpipe()
    // subprocess() -> fork(); exec(); wait()
    // TEST(subprocess("echo foo") == 0);

    return tests_failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
