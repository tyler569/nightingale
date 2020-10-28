#include <basic.h>
#include <ng/fs.h>
#include <ng/procfile.h>
#include <assert.h>
#include <stdlib.h>

ssize_t procfs_read(struct open_file *ofd, void *data, size_t len) {
        return -1;
}

void procfs_close(struct open_file *ofd) {
}

struct file *make_procfile(const char *name, void (*fn)(struct open_file *), void *data) {
        return NULL;
}

void procfs_init() {
}