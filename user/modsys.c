#include <stdio.h>
#include <syscall.h>

int main(void) {
    return syscall0(101);
}
