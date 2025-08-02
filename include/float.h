#pragma once

/* Basic floating-point limits and characteristics */

/* Radix of exponent representation */
#define FLT_RADIX 2

/* Number of decimal digits of precision */
#define FLT_DIG 6
#define DBL_DIG 15
#define LDBL_DIG 18

/* Number of base-FLT_RADIX digits in the significand */
#define FLT_MANT_DIG 24
#define DBL_MANT_DIG 53
#define LDBL_MANT_DIG 64

/* Minimum int x such that 10^x is a normalized float */
#define FLT_MIN_10_EXP (-37)
#define DBL_MIN_10_EXP (-307)
#define LDBL_MIN_10_EXP (-4931)

/* Maximum int x such that 10^x is a representable finite float */
#define FLT_MAX_10_EXP 38
#define DBL_MAX_10_EXP 308
#define LDBL_MAX_10_EXP 4932

/* Minimum int x such that FLT_RADIX^(x-1) is a normalized float */
#define FLT_MIN_EXP (-125)
#define DBL_MIN_EXP (-1021)
#define LDBL_MIN_EXP (-16381)

/* Maximum int x such that FLT_RADIX^(x-1) is a representable finite float */
#define FLT_MAX_EXP 128
#define DBL_MAX_EXP 1024
#define LDBL_MAX_EXP 16384

/* Maximum representable finite floating-point number */
#define FLT_MAX 3.40282347e+38F
#define DBL_MAX 1.7976931348623157e+308
#define LDBL_MAX 1.18973149535723176502e+4932L

/* Smallest normalized positive floating-point number */
#define FLT_MIN 1.17549435e-38F
#define DBL_MIN 2.2250738585072014e-308
#define LDBL_MIN 3.36210314311209350626e-4932L

/* Smallest positive floating-point number x such that 1.0 + x != 1.0 */
#define FLT_EPSILON 1.19209290e-07F
#define DBL_EPSILON 2.2204460492503131e-16
#define LDBL_EPSILON 1.08420217248550443401e-19L