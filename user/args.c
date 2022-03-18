#include <stdio.h>

int main(int argc, char **argv)
{
    printf("There are %i arguments (according to argc)\n", argc);
    printf("argv is %p\n", (void *)argv);
    if (argv == 0) {
        printf("Let's not dereference it\n");
        return 1;
    }

    for (int i = 0; i < argc; i++) {
        if (argv[i] == 0) {
            printf("c: arg %i = (null)", i);
            break;
        }
        printf("arg %i = \"%s\"\n", i, argv[i]);
    }

    printf("---\n");

    int count = 0;
    for (char **c = argv; *c != 0; c++) {
        printf("v: arg %i = \"%s\"\n", count, *c);
        count++;
    }
    printf("There are %i arguments (looking at argv)\n", count);

    return 0;
}
