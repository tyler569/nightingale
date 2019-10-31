
#ifndef _ASSERT_H_
#define _ASSERT_H_

#include <stdio.h>

#ifndef NDEBUG

#define assert(assertion)                                                      \
        do {                                                                   \
                if (!(assertion)) {                                            \
                        printf("[ASSERT] '" #assertion "'\n");                 \
                }                                                              \
        } while (0)
#else
#define assert(...)
#endif

#endif
