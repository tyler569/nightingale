#pragma once

/*
 * stdbool.h - Boolean type for Nightingale OS
 */

#ifndef __cplusplus

#define bool _Bool
#define true 1
#define false 0

#else /* __cplusplus */

/* C++ has built-in bool */

#endif /* !__cplusplus */

#define __bool_true_false_are_defined 1