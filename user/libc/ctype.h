
#pragma once
#ifndef _CTYPE_H_
#define _CTYPE_H_

#include <stdbool.h>

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

int toupper(int c);
int tolower(int c);

#endif
