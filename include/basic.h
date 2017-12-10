
#pragma once
#ifndef NIGHTINGALE_BASIC_H
#define NIGHTINGALE_BASIC_H

_Static_assert(__STDC_VERSION__ >= 201112L, "Nightingale must be compiled as C11 or greater");
_Static_assert(__STDC_HOSTED__ != 1, "Nightingale must not be compiled in a hosted environment");

// Integer types

_Static_assert(__CHAR_BIT__ == 8, "Bytes must be 8 bits (How did you even do this?)");
_Static_assert(sizeof(short int) == 2, "Short must be 2 bytes");
_Static_assert(sizeof(int) == 4, "Int must be 4 bytes");
_Static_assert(sizeof(long int) == 8, "Long must be 8 bytes");
_Static_assert(sizeof(void *) == 8, "Pointer must be 8 bytes (Are you using a 32 bit compiler?)");

typedef unsigned char u8;
typedef unsigned short int u16;
typedef unsigned int u32;
typedef unsigned long int u64;
typedef unsigned long int usize;

typedef signed char i8;
typedef signed short int i16;
typedef signed int i32;
typedef signed long int i64;
typedef signed long int isize;

// Boolean type

typedef _Bool bool;
#define true 1
#define false 0

// General stuff
#define NULL (void *)0

#endif

