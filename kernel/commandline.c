#include <ng/limine.h>
#include <ng/string.h>
#include <stdlib.h>
#include <string.h>

static int n_arguments;
static char *kernel_command_line;

void init_command_line() {
	kernel_command_line = limine_kernel_command_line();

	for (char *cursor = kernel_command_line; *cursor; cursor++) {
		if (*cursor == ' ') {
			*cursor = 0;
			n_arguments++;
		}

		if (*cursor == '"') {
			cursor++;
			while (*cursor && *cursor != '"') {
				cursor++;
			}
		}
	}
	n_arguments++;
}

const char *get_kernel_argument(const char *key) {
	const char *cursor = kernel_command_line;

	for (int i = 0; i < n_arguments; i++) {
		if (strccmp(key, cursor, '=') == 0) {
			return strchr(cursor, '=') + 1;
		}

		cursor = strchr(cursor, 0) + 1;
	}
	return NULL;
}
