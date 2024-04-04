#include <stdio.h>
#include <unistd.h>

char buffer[1024];

int main() {
	getcwd(buffer, 1024);
	printf("%s\n", buffer);
}
