
#ifndef NIGHTINGALE_STRING_H
#define NIGHTINGALE_STRING_H

#include <basic.h>

bool isalnum(char c);
bool isalpha(char c);
bool islower(char c);
bool isupper(char c);
bool isdigit(char c);
bool isxdigit(char c);
bool iscntrl(char c);
bool isspace(char c);
bool isblank(char c);
bool isprint(char c);
bool ispunct(char c);

char *strcpy(char *dest, char *src);
char *strncpy(char *dest, char *src, usize count);
usize strlen(const char *s);
int strcmp(const char *a, const char *b);
int strncmp(const char *a, const char *b, usize count);
char *strchr(char *s, char c);
char *strcat(char *restrict dest, const char *restrict src);
char *strncat(char *restrict dest, const char *restrict src, size_t max);

void *memchr(void *mem_, u8 v, usize count);
int memcmp(const void *a_, const void *b_, usize count);
void *memset(void *dest_, u8 value, usize count);
void *wmemset(void *dest_, u16 value, usize count);
void *lmemset(void *dest_, u32 value, usize count);
void *qmemset(void *dest_, u64 value, usize count);
void *memcpy(void *restrict dest_, const void *restrict src_, usize count);
void *memmove(void *dest_, const void *src_, usize count);

#endif

