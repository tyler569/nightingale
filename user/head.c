#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#define BUF 1024

char buffer[BUF];

void copy_n_lines(FILE *out, FILE *in, int lines)
{
    int newlines_seen = 0;
    while (newlines_seen < lines) {
        char *c = fgets(buffer, BUF, in);
        if (!c) {
            perror("fgets");
            exit(1);
        }
        int len = strlen(buffer);
        int n = fwrite(buffer, 1, len, out);
        if (n < 0) {
            perror("fwrite");
            exit(1);
        }
        if (memchr(buffer, '\n', len)) {
            newlines_seen++;
        }
    }
}

void copy_n_characters(FILE *out, FILE *in, int chars)
{
    int n = 0;
    int c;
    while (n++ < chars && (c = getc(in)) != EOF) {
        putc(c, out);
    }
}

int main(int argc, char **argv)
{
    int n_c = -1;
    int n_nl = 10;

    int c;
    while ((c = getopt(argc, argv, "n:c:")) != -1) {
        switch (c) {
        case 'n':
            n_nl = atoi(optarg);
            break;
        case 'c':
            n_c = atoi(optarg);
            n_nl = -1;
            break;
        default:
            fprintf(stderr, "oops\n");
        }
    }

    if (n_c > 0) {
        copy_n_characters(stdout, stdin, n_c);
    } else {
        copy_n_lines(stdout, stdin, n_nl);
    }

    return 0;
}
