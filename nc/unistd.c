
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int isatty(int fd) {
        // does not account for piped stdins.
        return fd < 3;
}


/*
char __execve_page[4096] = {0};
char *__execve_page_end = __execve_page + 4096;
int execve(const char *path, char *const argv[], char *const envp[]) {
        if (envp != NULL) {
                printf("enpv is not currently supported, ignoring\n");
        }

        char *moved_argv[32] = {0};

        int argc = 0;
        char *cursor = __execve_page;
        while (*argv) {
                int can_read_bytes = (int)(__execve_page_end - cursor - 1);
                if (can_read_bytes < 2) {
                        printf("warning: argv exceeded maximum length\n");
                        break;
                }
                moved_argv[argc] = cursor;
                cursor = strncpy(cursor, *argv, can_read_bytes) + 1;
        }

        ng_execve(path, __execve_page, envp);
}
*/

int execvp(const char *path, char *const argv[]) {
        return execve(path, argv, NULL);
}

off_t lseek(int fd, off_t offset, int whence) {
        return seek(fd, offset, whence);
}

