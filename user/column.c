#include <errno.h>
#include <list.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef __nightingale__
void *zmalloc(size_t len) {
	void *allocation = malloc(len);
	memset(allocation, 0, len);
	return allocation;
}
#endif

#define MAX_LENGTH 110

list strings = LIST_INIT(strings);

struct str {
	list_node node;
	char string[MAX_LENGTH];
};

int main(int argc, char **argv) {
	if (argc > 1) {
		fprintf(stderr, "usage: column\n");
		exit(1);
	}

	int count = 0;

	while (!feof(stdin)) {
		struct str *str = zmalloc(sizeof(struct str));
		char *err = fgets(str->string, MAX_LENGTH, stdin);
		if (err == NULL && !feof(stdin)) {
			perror("fgets");
			exit(1);
		}
		count++;
		if (str->string[0])
			list_append(&strings, &str->node);
	}

	if (count == 0)
		return EXIT_SUCCESS;

	int max_len = 0;
	list_for_each_safe (&strings) {
		struct str *str = container_of(struct str, node, it);
		int len = strlen(str->string);

		// drop newlines
		if (len > 0 && str->string[len - 1] == '\n') {
			str->string[len - 1] = '\0';
			len -= 1;
		}

		max_len = len > max_len ? len : max_len;
	}

	int screen_width = 80;
	int columns = screen_width / (max_len + 1);
	if (columns < 1)
		columns = 1;

	int column_width = 80 / columns;
	int i = 0;
	int last_newline = 0;

	list_for_each_safe (&strings) {
		struct str *str = container_of(struct str, node, it);
		printf("%-*s", column_width, str->string);
		last_newline = 0;
		i++;
		if (i > 0 && i % columns == 0) {
			printf("\n");
			last_newline = 1;
		}
	}
	if (!last_newline)
		printf("\n");

	return EXIT_SUCCESS;
}
