
#include <stdio.h>
#include <unistd.h>

int forkstuff() {
        while (true) {
                for (int i=0; i<10000000; i++);

                printf("doing thread things\n");
        }
}
        

int main() {
        for (int i=0; i<10; i++) {
                if (!fork())
                        forkstuff();
        }

        for (int i=0; i < 50000000; i++);

        return 0;
}
