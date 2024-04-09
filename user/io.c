#include "ng/submission_q.h"
#include "ng/syscall_consts.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

struct q {
	struct submission *q;
	int len;
	int capacity;
};

struct q new_q();
void delete_q(struct q *);

int io_open(struct q *q, const char *path, int flags, mode_t mode);
int io_write(struct q *q, int fd, const void *buffer, size_t len);
int io_close(struct q *q, int fd);
int io_submit(struct q *q);

int main() {
	struct q q_ = new_q();
	struct q *q = &q_;

	int v = io_open(q, "io_file", O_CREAT | O_WRONLY | O_TRUNC, 0644);
	io_write(q, v, "Hello World\n", 12);
	io_close(q, v);

	int err = io_submit(q);
	if (err < 0)
		perror("submit");

	delete_q(q);
}

struct q new_q() {
	struct q q;
	q.q = calloc(16, sizeof(struct submission));
	q.len = 0;
	q.capacity = 16;
	return q;
}

void delete_q(struct q *q) { free(q->q); }

int result_number = 0;

int io_open(struct q *q, const char *path, int flags, mode_t mode) {
	q->q[q->len++] = (struct submission) { NG_OPENAT,
		{ AT_FDCWD, (uintptr_t)path, flags, mode } };
	return -q->len;
}

int io_write(struct q *q, int fd, const void *buffer, size_t len) {
	q->q[q->len++]
		= (struct submission) { NG_WRITE, { fd, (uintptr_t)buffer, len } };
	return -q->len;
}

int io_close(struct q *q, int fd) {
	q->q[q->len++] = (struct submission) { NG_CLOSE, { fd } };
	return -q->len;
}

int __ng_submit(struct submission *, size_t);

int io_submit(struct q *q) { return __ng_submit(q->q, q->len); }
