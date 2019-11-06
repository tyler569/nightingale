
#include <basic.h>
#include <ng/fs.h>
#include <ng/thread.h>
#include <ng/ringbuf.h>
#include <ng/syscall.h>
#include <nc/stdlib.h>
#include <nc/string.h>

int pipe_close(struct open_fd *n) {
        n->node->refcnt--;
        n->node->signal_eof = 1;
        if (n->node->refcnt == 1) {
                wake_blocked_threads(&n->node->blocked_threads);
        }
        if (n->node->refcnt <= 0) {
                free(n->node);
                n->node = 0;
        }
        return 0;
}

ssize_t pipe_read(struct open_fd *n, void *data, size_t len) {
        len = ring_read(&n->node->ring, data, len);
        if (len == 0)  return -1;
        return len;
}

ssize_t pipe_write(struct open_fd *n, const void *data, size_t len) {
        len = ring_write(&n->node->ring, data, len);
        wake_blocked_threads(&n->node->blocked_threads);
        return len;
}

sysret sys_pipe(int pipefd[static 2]) {
        // create a pipe
        // create 2 open_fds, one for each end
        // put them in the output
        
        struct fs_node *pipe_node = zmalloc(sizeof(struct fs_node));
        struct open_fd *readfd = zmalloc(sizeof(struct open_fd));
        struct open_fd *writefd = zmalloc(sizeof(struct open_fd));

        pipe_node->filetype = PIPE;
        strcpy(pipe_node->filename, "<pipe>");
        pipe_node->refcnt = 2; // don't free until both ends are closed
        pipe_node->permission = 0;
        pipe_node->uid = 0; // running_process->euid
        pipe_node->ops.close = pipe_close;
        pipe_node->ops.read = pipe_read;
        pipe_node->ops.write = pipe_write;

        emplace_ring(&pipe_node->ring, 4096);

        // pipe_node has no parent and does not exist in the normal
        // directory tree.

        readfd->node = pipe_node;
        writefd->node = pipe_node;
        readfd->flags = USR_READ;
        writefd->flags = USR_WRITE;

        pipefd[0] = dmgr_insert(&running_process->fds, readfd);
        pipefd[1] = dmgr_insert(&running_process->fds, writefd);

        return 0;
}

