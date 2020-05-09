
#include <basic.h>
#include <ng/fs.h>
#include <ng/thread.h>
#include <ng/ringbuf.h>
#include <ng/signal.h>
#include <ng/syscall.h>
#include <nc/stdlib.h>
#include <nc/string.h>

void pipe_close(struct open_file *n) {
        if (n->flags & USR_WRITE && n->node->refcnt < 3)
                n->node->signal_eof = 1;

        if (n->node->refcnt == 2) {
                wake_blocked_threads(&n->node->blocked_threads);
        }
        if (n->node->refcnt <= 0) {
                free_ring(&n->node->ring);
                free(n->node);
                n->node = 0;
        }
}

ssize_t pipe_read(struct open_file *n, void *data, size_t len) {
        len = ring_read(&n->node->ring, data, len);
        if (len == 0)  return -1;
        return len;
}

ssize_t pipe_write(struct open_file *n, const void *data, size_t len) {
        if (n->node->refcnt < 2) {
                signal_self(SIGPIPE);
        }
        len = ring_write(&n->node->ring, data, len);
        wake_blocked_threads(&n->node->blocked_threads);
        return len;
}

sysret sys_pipe(int pipefd[static 2]) {
        // create a pipe
        // create 2 open_files, one for each end
        // put them in the output
        
        struct file *pipe_node = zmalloc(sizeof(struct file));
        struct open_file *readfd = zmalloc(sizeof(struct open_file));
        struct open_file *writefd = zmalloc(sizeof(struct open_file));

        pipe_node->filetype = FT_PIPE;
        strcpy(pipe_node->filename, "<pipe>");
        pipe_node->refcnt = 2; // don't free until both ends are closed
        pipe_node->permissions = 0;
        pipe_node->uid = 0; // running_process->euid
        pipe_node->close = pipe_close;
        pipe_node->read = pipe_read;
        pipe_node->write = pipe_write;

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

