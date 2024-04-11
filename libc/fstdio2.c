#include <ng/common.h>
#include <stream.h>

typedef struct stream FILE;

static ssize_t write_data_to_stream(FILE *, const void *data, size_t size);
static ssize_t read_line_from_stream(FILE *, void *data, size_t size);
static ssize_t read_data_from_stream(FILE *, void *data, size_t size);

ssize_t fwrite2(const void *ptr, size_t size, size_t nmemb, FILE *f) {
	size_t total = size * nmemb;

	switch (f->buffer_mode) {
	case STREAM_BUFFER_NONE:
		return F_WRITE(f, ptr, total);
	case STREAM_BUFFER_LINE:
	case STREAM_BUFFER_FULL:
		return write_data_to_stream(f, ptr, total);
	}
	return 0;
}

ssize_t fread2(void *ptr, size_t size, size_t nmemb, FILE *f) {
	size_t total = size * nmemb;

	switch (f->buffer_mode) {
	case STREAM_BUFFER_NONE:
		return F_READ(f, ptr, total);
	case STREAM_BUFFER_LINE:
		return read_line_from_stream(f, ptr, total);
	case STREAM_BUFFER_FULL:
		return read_data_from_stream(f, ptr, total);
	}
}
