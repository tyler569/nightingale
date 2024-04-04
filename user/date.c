#include <stdio.h>
#include <time.h>

int main() {
	time_t t;
	struct tm tm;

	btime(&t, &tm);

	char buffer[128] = { 0 };
	strftime(buffer, 128, "now (utc): %A / %c%n", &tm);
	printf("%li %s", t, buffer);
	printf("or\n");
	strftime(buffer, 128, "%A %B %d, %Y at %r (%FT%T)", &tm);
	printf("%s\n", buffer);
}
