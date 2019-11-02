
#pragma once
#ifndef NG_STRING_H
#define NG_STRING_H

#include <ng/basic.h>
#include <nc/ctype.h>
#include <nc/string.h>

const char *str_until(const char *source, char *tok, char *delims);

#endif // NG_STRING_H

