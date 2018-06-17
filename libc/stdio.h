
#pragma once
#ifndef _STDIO_H_
#define _STDIO_H_

#include <stddef.h>

#define stdout 1

size_t puts(const char *str);
size_t printf(const char *format, ...);

#endif
