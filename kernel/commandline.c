#include <ng/multiboot.h>
#include <ng/string.h>
#include <stdlib.h>
#include <string.h>

static int n_arguments;
static char *kernel_command_line;

void init_command_line()
{
    kernel_command_line = mb_cmdline();

    char in_quote = 0;
    for (char *cursor = kernel_command_line;; cursor += 1) {
        if (*cursor == 0) {
            n_arguments += 1;
            break;
        }

        if (in_quote == 0 && (*cursor == '"' || *cursor == '\'')) {
            in_quote = *cursor;
        } else if (*cursor == in_quote) {
            in_quote = 0;
        }

        if (in_quote == 0 && *cursor == ' ') {
            *cursor = 0;
            n_arguments += 1;
        }
    }
}

const char *get_kernel_argument(const char *key)
{
    const char *cursor = kernel_command_line;

    for (int i = 0; i < n_arguments; i++) {
        if (strccmp(key, cursor, '=') == 0) {
            return strchr(cursor, '=') + 1;
        }

        cursor = strchr(cursor, 0) + 1;
    }
    return NULL;
}
