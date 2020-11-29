#pragma once
#ifndef NG_STRING_H
#define NG_STRING_H

#include <basic.h>
#include <ctype.h>
#include <string.h>

const char *str_until(const char *source, char *tok, const char *delims);

char *strcpyto(char *dest, const char *source, char delim);
char *strncpyto(char *dest, const char *source, size_t len, char delim);

#endif // NG_STRING_H
