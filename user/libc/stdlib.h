
#ifndef _STDLIB_H_
#define _STDLIB_H_

void* malloc(size_t len);
void free(void* alloc);
void* realloc(void* alloc, size_t len);
void* calloc(size_t count, size_t len);

#endif

