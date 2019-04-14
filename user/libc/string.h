
#pragma once
#ifndef _STRING_H_
#define _STRING_H_

#include <sys/types.h>

char* strcpy(char* dest, char* src);
char* strncpy(char* dest, char* src, size_t count);
size_t strlen(const char* s);
int strcmp(const char* a, const char* b);
int strncmp(const char* a, const char* b, size_t count);
const char* strchr(const char* s, int c);
const char* strstr(const char* s, const char* flag);
void* memchr(void* mem_, int v, size_t count);
int memcmp(const void* a_, const void* b_, size_t count);
void* memset(void* dest_, unsigned char value, size_t count);

#ifdef __NG_LIBC_WIDE_MEMSET
void* wmemset(void* dest_, unsigned short value, size_t count);
void* lmemset(void* dest_, unsigned int value, size_t count);
void* qmemset(void* dest_, unsigned long value, size_t count);
#endif

void* memcpy(void* restrict dest_, const void* restrict src_, size_t count);
void* memmove(void* dest_, const void* src_, size_t count);

#endif

