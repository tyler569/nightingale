
#include <stdio.h>
#include <unistd.h>

void print_my_letter(char c) {
        for (int j = 0; j < 10; j++) {
                for (int i = 0; i < 1000000; i++) {
                }
                printf("%c", c);
        }
        exit(0);
}

int main() {
        for (int i = 0; i < 5; i++) {
                for (char c = '@'; c < 'z'; c++) {
                        if (!fork()) {
                                print_my_letter(c);
                        }
                }
        }

        print_my_letter('z');

        return 0;
}
