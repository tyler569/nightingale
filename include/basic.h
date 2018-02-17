
#pragma once
#ifndef NIGHTINGALE_BASIC_H
#define NIGHTINGALE_BASIC_H

#define static_assert _Static_assert

static_assert(__STDC_VERSION__ >= 201112L, "Nightingale must be compiled as C11 or greater");
static_assert(__STDC_HOSTED__ != 1, "Nightingale must not be compiled in a hosted environment");

// Numeric types
#define NO_FLOAT

static_assert(__CHAR_BIT__ == 8, "Bytes must be 8 bits (How did you even do this?)");
static_assert(sizeof(short int) == 2, "Short must be 2 bytes");
static_assert(sizeof(int) == 4, "Int must be 4 bytes");
static_assert(sizeof(long int) == 8, "Long must be 8 bytes");
static_assert(sizeof(void *) == 8, "Pointer must be 8 bytes (Are you using a 32 bit compiler?)");

typedef unsigned char       u8;
typedef unsigned short int  u16;
typedef unsigned int        u32;
typedef unsigned long int   u64;
typedef unsigned long int   usize;
typedef unsigned long int   size_t;
typedef __uint128_t         u128;

typedef signed char         i8;
typedef signed short int    i16;
typedef signed int          i32;
typedef signed long int     i64;
typedef signed long int     isize;
typedef __int128_t          i128;

#ifndef NO_FLOAT
typedef float f32
typedef double f64
#endif

// Boolean type

typedef _Bool bool;
enum {
    false = 0,
    true = 1,
};

// General stuff
#define NULL (void *)0
#define asm __asm__
#define PACKED __attribute__((packed))

#endif

