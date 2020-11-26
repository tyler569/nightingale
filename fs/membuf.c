#include <basic.h>
#include <assert.h>
#include <errno.h>
#include <ng/fs.h>

ssize_t membuf_read(struct open_file *n, void *data, size_t len);
ssize_t membuf_write(struct open_file *n, const void *data, size_t len);
off_t membuf_seek(struct open_file *n, off_t offset, int whence);
void membuf_close(struct open_file *n);

struct file_ops membuf_ops = {
    .read = membuf_read,
    .write = membuf_write,
    .seek = membuf_seek,
};

ssize_t membuf_read(struct open_file *ofd, void *data, size_t len) {
    struct file *n = ofd->node;
    assert(n->filetype == FT_BUFFER);
    struct membuf_file *membuf = (struct membuf_file *)n;

    ssize_t to_read = min(len, n->len - ofd->off);
    if (to_read < 0) { return 0; }

    memcpy(data, (char *)membuf->memory + ofd->off, to_read);
    ofd->off += to_read;

    return to_read;
}

ssize_t membuf_write(struct open_file *ofd, const void *data, size_t len) {
    struct file *file = ofd->node;
    assert(file->filetype == FT_BUFFER);
    struct membuf_file *membuf = (struct membuf_file *)file;

    if (file->len + len > membuf->capacity) {
        if (membuf->capacity == -1) { return -EPERM; }
        void *memory = realloc(membuf->memory, membuf->capacity * 2 + len);
        membuf->memory = memory;
        membuf->capacity *= 2;
        membuf->capacity += len;
    }

    memcpy((char *)membuf->memory + file->len, data, len);
    file->len += len;
    ofd->off = file->len;

    return len;
}

off_t membuf_seek(struct open_file *ofd, off_t offset, int whence) {
    struct file *file = ofd->node;
    assert(file->filetype == FT_BUFFER);
    struct membuf_file *membuf = (struct membuf_file *)file;

    switch (whence) {
    case SEEK_SET: ofd->off = offset; break;
    case SEEK_CUR: ofd->off += offset; break;
    case SEEK_END: ofd->off = file->len + offset; break;
    default:
        // screened before - shouldn't be possible?
        return -EINVAL;
    }

    return ofd->off;
}

void membuf_close(struct open_file *ofd) {
    struct file *file = ofd->node;
    assert(file->filetype == FT_BUFFER);
    struct membuf_file *membuf = (struct membuf_file *)file;

    file->refcnt--;
    if (file->refcnt <= 0) { free(membuf->memory); }
}

struct membuf_file *__create_file(int mode) {
    struct membuf_file *new = zmalloc(sizeof(struct membuf_file));

    new->file.filetype = FT_BUFFER;
    new->file.refcnt = 0;
    new->file.flags = 0;
    new->file.permissions = mode;
    new->file.len = 0;
    new->file.ops = &membuf_ops;
    return new;
}

struct file *create_file(struct file *directory, const char *filename,
                         int mode) {
    assert(directory->filetype == FT_DIRECTORY);
    char *allocated_filename = malloc(strlen(filename));
    strcpy(allocated_filename, filename);

    struct membuf_file *new = __create_file(mode);
    new->memory = zmalloc(1024);
    new->capacity = 1024;
    add_dir_file(directory, &new->file, allocated_filename);

    return &new->file;
}

struct file *make_tar_file(const char *name, size_t len, void *data) {
    struct membuf_file *new = __create_file(0444);
    new->memory = data;
    new->capacity = -1;
    new->file.len = len;

    return &new->file;
}
