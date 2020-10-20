#pragma once
#ifndef NG_STRING_H
#define NG_STRING_H

#include <basic.h>
#include <ctype.h>
#include <string.h>

const char *str_until(const char *source, char *tok, char *delims);

#endif // NG_STRING_H
