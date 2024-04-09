#pragma once

#include <sys/cdefs.h>
#include <sys/types.h>

BEGIN_DECLS

char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, size_t count);
char *strcat(char *dest, const char *src);
char *strncat(char *dest, const char *src, size_t count);
char *strdup(const char *str);
char *strndup(const char *str, size_t len);
size_t strlen(const char *s);
int strcmp(const char *a, const char *b);
int strncmp(const char *a, const char *b, size_t count);
char *strchr(const char *s, int c);
char *strrchr(const char *s, int c);
char *strstr(const char *s, const char *flag);
char *strpbrk(const char *s, const char *accept);
void *memchr(const void *pm, int v, size_t count);
int memcmp(const void *pa, const void *pb, size_t count);
void *memset(void *pdest, int value, size_t count);

#ifdef _NC_WIDE_MEMSET
void *wmemset(void *pdest, unsigned short value, size_t count);
void *lmemset(void *pdest, unsigned int value, size_t count);
void *qmemset(void *pdest, unsigned long value, size_t count);
#endif

void *memcpy(void *restrict dest, const void *restrict src, size_t n);
void *memccpy(void *restrict dest, const void *restrict src, int c, size_t n);
void *memmove(void *pdest, const void *psrc, size_t count);
size_t strspn(const char *s, const char *accept);

// TODO
size_t strcspn(const char *s, const char *reject);
int strcoll(const char *s1, const char *s2);

END_DECLS
