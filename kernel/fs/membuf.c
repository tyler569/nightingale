
#include <basic.h>
#include <assert.h>
#include "vfs.h"
#include "membuf.h"

ssize_t membuf_read(struct fs_node* n, void* data, size_t len) {
    assert(n->filetype = MEMORY_BUFFER, "oops");

    ssize_t to_read = min(len, n->len - n->off);
    if (to_read < 0) {
        return 0;
    }

    memcpy(data, (char*)n->extra_data + n->off, to_read);
    n->off += to_read;

    return to_read;
}

ssize_t membuf_write(struct fs_node* n, const void* data, size_t len) {
    assert(n->filetype = MEMORY_BUFFER, "oops");

    return 0; // TODO
}

off_t membuf_seek(struct fs_node* n, off_t offset, int whence) {
    assert(n->filetype = MEMORY_BUFFER, "oops");

    switch (whence) {
    case SEEK_SET:
        n->off = offset;
        break;
    case SEEK_CUR:
        n->off += offset;
        break;
    case SEEK_END:
        n->off = n->len + offset;
        break;
    // default:
        // screened before - shouldn't be possible?
    }

    return n->off;
}

