
#include <ng/basic.h>
#include "membuf.h"
#include <ng/panic.h>
#include <ng/fs.h>

ssize_t membuf_read(struct open_fd *ofd, void *data, size_t len) {
        struct fs_node *n = ofd->node;
        assert(n->filetype = MEMORY_BUFFER, "oops");

        ssize_t to_read = min(len, n->len - ofd->off);
        if (to_read < 0) {
                return 0;
        }

        memcpy(data, (char *)n->memory + ofd->off, to_read);
        ofd->off += to_read;

        return to_read;
}

ssize_t membuf_write(struct open_fd *n, const void *data, size_t len) {
        assert(n->node->filetype = MEMORY_BUFFER, "oops");

        return 0; // TODO
}

off_t membuf_seek(struct open_fd *n, off_t offset, int whence) {
        assert(n->node->filetype = MEMORY_BUFFER, "oops");

        switch (whence) {
        case SEEK_SET:
                n->off = offset;
                break;
        case SEEK_CUR:
                n->off += offset;
                break;
        case SEEK_END:
                n->off = n->node->len + offset;
                break;
                // default:
                // screened before - shouldn't be possible?
        }

        return n->off;
}

