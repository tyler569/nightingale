#include <stdio.h>
#include <syscall.h>

int main(void) {
    return __syscall0(101);
}
