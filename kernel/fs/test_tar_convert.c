
#include <stdio.h>

long convert(char *thing) {
    long v = 0;

    for (int p=0; p<8; p++) {
        v += (thing[p] - '0') << ((7 - p) * 3);
    }

    return v;
}

int main() {
    char *tar_num = "00000013";
    printf("tar_number_convert(\"%s\") = %li\n", tar_num, convert(tar_num));

    tar_num = "00000010";
    printf("tar_number_convert(\"%s\") = %li\n", tar_num, convert(tar_num));
    tar_num = "10000000";
    printf("tar_number_convert(\"%s\") = %li\n", tar_num, convert(tar_num));
    tar_num = "00000020";
    printf("tar_number_convert(\"%s\") = %li\n", tar_num, convert(tar_num));
    tar_num = "00000017";
    printf("tar_number_convert(\"%s\") = %li\n", tar_num, convert(tar_num));
    tar_num = "00000100";
    printf("tar_number_convert(\"%s\") = %li\n", tar_num, convert(tar_num));
    tar_num = "00000000";
    printf("tar_number_convert(\"%s\") = %li\n", tar_num, convert(tar_num));

}

