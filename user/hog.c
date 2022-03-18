#include <unistd.h>

int main(int argc, char **argv)
{
    if (!fork()) {
        while (true)
            ;

        return 0;
    }
    return 0;
}
