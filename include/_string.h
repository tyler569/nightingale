
#pragma once
#ifndef NIGHTINGALE_STRING_H
#define NIGHTINGALE_STRING_H
#include <basic.h>
     
i32 memcmp(const void *, const void *, usize);

void *memcpy(void *restrict, const void *restrict, usize);
void *memmove(void *, const void *, usize);

void *memset(void *, u8, usize);
void *wmemset(void *, u16, usize);
void *lmemset(void *, u32, usize);
void *qmemset(void *, u64, usize);

usize strlen(const char *);
 
#endif
