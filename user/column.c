
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#ifndef __nightingale__
void *zmalloc(size_t len) {
        void *allocation = malloc(len);
        memset(allocation, 0, len);
        return allocation;
}
#endif

#define MAX_STRINGS 1024
#define MAX_LENGTH 256

int main(int argc, char **argv) {
        if (argc > 1) {
                fprintf(stderr, "usage: column\n");
                exit(1);
        }

        char **strings = zmalloc(MAX_STRINGS * sizeof(char *));
        int count = 0;

        while (!feof(stdin) && count < MAX_STRINGS) {
                char *str = zmalloc(MAX_LENGTH);
                char *err = fgets(str, MAX_LENGTH, stdin);
                if (err == NULL && !feof(stdin)) {
                        perror("fgets");
                        exit(1);
                }

                strings[count] = str;
                count += 1;
        }

        if (count == 0) {
                return EXIT_SUCCESS;
        }

        int max_len = 0;
        for (int i=0; i<count; i++) {
                int len = strlen(strings[i]);

                // drop newlines
                if (len > 0 && strings[i][len-1] == '\n') {
                        strings[i][len-1] = '\0';
                        len -= 1;
                }

                if (len > max_len)  max_len = len;
        }

        int screen_width = 80;
        int columns = screen_width / (max_len + 1);
        if (columns < 1) {
                columns = 1;
        }

        int column_width = 80 / columns;

        for (int i=0; i<count; i++) {
                if ((i+1) % columns == 0) {
                        printf("\n");
                }

                printf("%-*s", column_width, strings[i]);
        }

        printf("\n");
        return EXIT_SUCCESS;
}

