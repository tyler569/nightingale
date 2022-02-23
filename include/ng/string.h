#pragma once
#ifndef NG_STRING_H
#define NG_STRING_H

#include <basic.h>
#include <ctype.h>
#include <string.h>

const char *str_until(const char *source, char *tok, const char *delims);

char *strcpyto(char *dest, const char *source, char delim);
char *strncpyto(char *dest, const char *source, size_t len, char delim);

// Copies at most `len` characters from `src` to `dest`, terminated by
// either a NUL byte or `c`. If there is space in `dest`, a NUL byte is
// appended to the copied data.
//
// Returns a pointer to the end of the copied region in the `src`
// string, either the NUL terminator, the `c` delimeter, or the last
// character coped to `dest`.
char *strcncpy(char *dest, const char *src, int c, size_t len);

// `strcmp`, but uses both NUL and `c` as string terminators;
//
// strccmp("abc;xyz", "abc;foo", ';') is TRUE
int strccmp(const char *a, const char *b, int c);

#endif // NG_STRING_H
