#include <stdio.h>
#include <syscall.h>

int main() {
	return __syscall0(101);
}
