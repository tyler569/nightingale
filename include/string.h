
#pragma once
#ifndef NIGHTINGALE_STRING_H
#define NIGHTINGALE_STRING_H
#include <basic.h>
     
i32 memcmp(const void *, const void *, usize);

void *memcpy(void *__restrict, const void *__restrict, usize);
void *memmove(void *, const void *, usize);

void *memset(void *, int, usize);
void *wmemset(void *, int, usize);

usize strlen(const char *);
 
#endif
