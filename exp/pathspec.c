
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PATH_MAX 256

struct pathspec {
    char data[PATH_MAX];
};

#define PATHSPEC_ROOT 1
#define PATHSPEC_SEP  2

struct pathspec *path_new() {
    return calloc(sizeof(struct pathspec), 1);
}

struct pathspec *path_root(struct pathspec *path) {
    memset(path->data, 0, PATH_MAX);
    path->data[0] = 1;
    return path;
}

struct pathspec *path_append(struct pathspec *path, const char *next) {
    if (!path) return NULL;

    size_t path_len = strlen(path->data);
    path->data[path_len] = 2;
    strncat(path->data, next, path_len - PATH_MAX);
    return path;
}

struct pathspec *path_concat(struct pathspec *path, struct pathspec *part) {
    // append `part` onto `path`
}

struct pathspec *path_parse(struct pathspec *path, const char *path_str) {
    // parse `path_str` into `path`
}

struct pathspec *path_relative(struct pathspec *path, const char *path_str) {
    // interepret `path_str` as relative to `path`
    // creates a new path, does not modify `path`
}

void path_print(struct pathspec *path) {
    char *cursor = path->data, *end;

    while (1) {
        switch (*cursor) {
        case 0:
            return;
        case 1:
            printf("<root>");
            cursor++;
            break;
        case 2:
            printf("/");
            cursor++;
            break;
        default:
            end = strchr(cursor, 2);
            if (!end) {
                printf("%s", cursor);
                return;
            }
            printf("%.*s", end - cursor, cursor);
            cursor = end;
        }
    }
}

int main() {
    struct pathspec *path = path_new();
    path_root(path);
    path_append(path, "bin");
    path_append(path, "src");
    path_append(path, "lib");
    path_print(path);
    printf("\n");

    path_parse(path, "/foo/bar/abc");
    print_path(path);
    printf("\n");
}
