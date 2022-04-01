#include <stdbool.h>
#include <stdio.h>
#include <math.h>

int main()
{
    double x = sin(4.0);

    // if (x > 0.0) {
    //     printf("ERROR ERROR\n");
    // }

    // printf("%lf\n", sin(4.0));

    while (true) {
        double pos = 20 * (sin(x) + 1.5);
        printf("%02i %lf %.*s*\n", (int)pos, pos, (int)(pos),
            "                                                                  "
            "         ");
        x += 0.1;
    }
    return 0;
}
