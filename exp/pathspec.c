
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

void path_free(struct pathspec *path) {
    free(path);
}

void path_zero(struct pathspec *path) {
    memset(path->data, 0, PATH_MAX);
}

void path_root(struct pathspec *path) {
    path_zero(path);
    path->data[0] = PATHSPEC_ROOT;
}

void path_copy(struct pathspec *copy, struct pathspec *original) {
    memcpy(copy->data, original->data, PATH_MAX);
}

struct pathspec *path_clone(struct pathspec *path) {
    struct pathspec *new = path_new();
    path_copy(new, path);
    return new;
}

void path_append(struct pathspec *path, const char *next) {
    if (!path) return;

    size_t path_len = strlen(path->data);
    if (path_len > 0) {
        path->data[path_len] = PATHSPEC_SEP;
        strncat(path->data, next, path_len - PATH_MAX);
    } else {
        strncpy(path->data, next, PATH_MAX);
    }
}

void path_concat(struct pathspec *path, struct pathspec *part) {
    strcat(path->data, "\2"); // PATHSPEC_SEP
    strcat(path->data, part->data);

    // or should this make a new pathspec?
}

struct special_file {
    char *name;
    int type;
};

void path_relative(struct pathspec *path, const char *path_str) {
    const char *s_cursor = path_str;
    char *cursor = path->data + strlen(path->data);
    char *end, *previous, *copy;
    size_t len;

    if (path_str[0] == '/') {
        path_root(path);
        cursor = path->data + 1;
        s_cursor = path_str + 1;
    }

    while (1) {
        switch (*s_cursor) {
        case 0:
            return;
        case '/':
            // *cursor++ = PATHSPEC_SEP;
            s_cursor++;
        default:
            end = strchr(s_cursor, '/');
            len = end - s_cursor;

            if (cursor != path->data) {
                *cursor++ = PATHSPEC_SEP;
            }

            if (end) {
                strncpy(cursor, s_cursor, len);
                cursor[len] = 0;
            } else {
                strcpy(cursor, s_cursor);
            }

            if (strcmp(cursor, ".") == 0) {
                *cursor = 0;
                cursor -= 1;
                *cursor = 0;
            } else if (strcmp(cursor, "..") == 0) {
                *cursor = 0;
                cursor -= 1;
                *cursor = 0;

                previous = strrchr(path->data, PATHSPEC_SEP);

                if (previous) {
                    cursor = previous;
                    *cursor = 0;
                } else if (*path->data == PATHSPEC_ROOT) {
                    cursor = path->data + 1;
                    *cursor = 0;
                } else {
                    cursor = path->data;
                    *cursor = 0;
                }
            } else {
                cursor = path->data + strlen(path->data);
            }

            s_cursor = end;

            if (s_cursor == NULL) {
                return;
            }
        }
    }
}

void path_parse(struct pathspec *path, const char *path_str) {
    path_zero(path);
    path_relative(path, path_str);
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
            end = strchr(cursor, PATHSPEC_SEP);
            if (!end) {
                printf("%s", cursor);
                return;
            }
            printf("%.*s", end - cursor, cursor);
            cursor = end;
        }
    }
}

void test_path_parse(const char *path_str) {
    struct pathspec path;
    path_zero(&path);
    path_parse(&path, path_str);
    printf("\"%s\" -> ", path_str);
    path_print(&path);
    printf("\n");
}

int main() {
    struct pathspec *path = path_new();
    path_root(path);
    path_append(path, "bin");
    path_append(path, "src");
    path_append(path, "lib");
    printf("original path: ");
    path_print(path);
    printf("\n");

    struct pathspec *part = path_new();
    path_append(part, "file.c");
    path_concat(path, part);

    printf("part: ");
    path_print(part);
    printf("\n");

    printf("final path: ");
    path_print(path);
    printf("\n");

    printf("parsing:\n");
    test_path_parse("/a/b/c");
    test_path_parse("a/b/c");
    test_path_parse("/a/b/c/..");
    test_path_parse("/a/b/c/./d");
    test_path_parse("/a/b/c/../d");
    test_path_parse("../../..");
    test_path_parse("/../../..");
    test_path_parse("a/../../..");
    test_path_parse("/a/../../..");
    test_path_parse("/home/tyler/Documents/projects/nightingale/kernel/../include/ng/fs.h");


    struct pathspec *working_dir = path_new();
    path_parse(working_dir, "/home/tyler");

    printf("working: ");
    path_print(working_dir);
    printf("\n");

    struct pathspec *file = path_clone(working_dir);
    path_relative(file, "../foobar/test.file");

    printf("file   : ");
    path_print(file);
    printf("\n");

    path_free(file);

    char buffer[1024];
    while (1) {
        printf("$ ");
        fgets(buffer, 1024, stdin);

        char *newl = strrchr(buffer, '\n');
        if (newl) {
            *newl = 0;
        }

        file = path_clone(working_dir);
        path_relative(file, buffer);

        printf("-> ");
        path_print(file);
        printf("\n");
    }
}
