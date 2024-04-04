#include <stdio.h>

int getc_unlocked(FILE *stream) { return getc(stream); }

int getchar_unlocked(void) { return getchar(); }

int putc_unlocked(int c, FILE *stream) { return putc(c, stream); }

int putchar_unlocked(int c) { return putchar(c); }

void clearerr_unlocked(FILE *stream) { clearerr(stream); }

int feof_unlocked(FILE *stream) { return feof(stream); }

int ferror_unlocked(FILE *stream) { return ferror(stream); }

int fileno_unlocked(FILE *stream) { return fileno(stream); }

int fflush_unlocked(FILE *stream) { return fflush(stream); }

int fgetc_unlocked(FILE *stream) { return fgetc(stream); }

int fputc_unlocked(int c, FILE *stream) { return fputc(c, stream); }

size_t fread_unlocked(void *ptr, size_t size, size_t n, FILE *stream) {
	return fread(ptr, size, n, stream);
}

size_t fwrite_unlocked(const void *ptr, size_t size, size_t n, FILE *stream) {
	return fwrite(ptr, size, n, stream);
}

char *fgets_unlocked(char *s, int n, FILE *stream) {
	return fgets(s, n, stream);
}

int fputs_unlocked(const char *s, FILE *stream) { return fputs(s, stream); }
