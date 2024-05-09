#pragma once

#include "stddef.h"
#include "sys/types.h"

struct stream;

struct stream_vtbl {
	ssize_t (*write)(struct stream *stream, const void *data, size_t size);
	ssize_t (*read)(struct stream *stream, void *data, size_t size);
	void (*close)(struct stream *stream);
	void (*flush)(struct stream *stream);
};

struct stream {
	const struct stream_vtbl *vtbl;
	void *context;
};

typedef struct stream FILE;

#define F_WRITE(s, d, sz) (s)->vtbl->write((s), (d), (sz))
#define F_READ(s, d, sz) (s)->vtbl->read((s), (d), (sz))
#define F_CLOSE(s) (s)->vtbl->close((s))
#define F_FLUSH(s) (s)->vtbl->flush((s))

extern struct stream *w_stdout;
