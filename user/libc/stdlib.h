
#ifndef _STDLIB_H_
#define _STDLIB_H_

#define EXIT_SUCCESS (0)
#define EXIT_FAILURE (1)

void* malloc(size_t len);
void free(void* alloc);
void* realloc(void* alloc, size_t len);
void* calloc(size_t count, size_t len);

#endif

