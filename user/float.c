#include <stdbool.h>
#include <stdio.h>
#include <math.h>

int main()
{
    double x = sin(4.0);

    while (true) {
        double pos = 20 * (sin(x) + 1.5);
        printf("%02i %lf ", (int)pos, pos);
        for (int i = 0; i < pos; i++)
            printf(" ");
        printf("*\n");
        x += 0.1;
    }
    return 0;
}
