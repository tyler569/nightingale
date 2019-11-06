
#include <basic.h>
#include <ng/panic.h>
#include <ng/fs.h>
#include <nc/errno.h>
#include "membuf.h"

ssize_t membuf_read(struct open_fd *ofd, void *data, size_t len) {
        struct fs_node *n = ofd->node;
        assert(n->filetype = MEMORY_BUFFER);

        ssize_t to_read = min(len, n->len - ofd->off);
        if (to_read < 0) {
                return 0;
        }

        memcpy(data, (char *)n->memory + ofd->off, to_read);
        ofd->off += to_read;

        return to_read;
}

ssize_t membuf_write(struct open_fd *ofd, const void *data, size_t len) {
        struct fs_node *node = ofd->node;
        assert(node->filetype = MEMORY_BUFFER);

        if (node->len + len > node->capacity) {
                if (node->capacity == -1)
                        return -EPERM;
                void *memory = realloc(node->memory, node->capacity * 2 + len);
                node->memory = memory;
                node->capacity *= 2;
                node->capacity += len;
        }

        memcpy((char *)node->memory + node->len, data, len);
        node->len += len;
        ofd->off = node->len;

        return len;
}

off_t membuf_seek(struct open_fd *n, off_t offset, int whence) {
        assert(n->node->filetype = MEMORY_BUFFER);

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
        default:
                // screened before - shouldn't be possible?
                panic_bt("oops");
        }

        return n->off;
}

