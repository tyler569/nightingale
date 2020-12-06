#include <basic.h>
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

long answer_count(char (*answers)[26]) {
    long count = 0;
    for (int i=0; i<26; i++) {
        count += (*answers)[i];
    }
    return count;
}

long one_group(FILE *stream) {
    char buffer[32];
    char answers[26] = {0};
    char tmp[26] = {0};
    int is_first = 1;
    while(fgets(buffer, 32, stream)) {
        if (strlen(buffer) < 2) break;

        for (int i=0; buffer[i]; i++) {
            if (!isalpha(buffer[i])) break;
            if (is_first)
                answers[buffer[i] - 'a'] = 1;
            else
                tmp[buffer[i] - 'a'] = 1;
        }
        if (!is_first) {
            for (int i=0; i<26; i++) {
                if (!tmp[i]) answers[i] = 0;
                tmp[i] = 0;
            }
        }
        is_first = 0;
    }
    return answer_count(&answers);
}

long total_yess(FILE *stream) {
    long yess = 0;
    while (!feof(stream)) {
        yess += one_group(stream);
    }
    return yess;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "argument required");
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("fopen");
        return 1;
    }

    printf("total yess: %li\n", total_yess(file));
}
