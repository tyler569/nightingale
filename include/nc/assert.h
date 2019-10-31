
#ifndef _ASSERT_H_
#define _ASSERT_H_

#include <stdio.h>

#ifdef DEBUG
#define assert(assertion)                                                      \
        do {                                                                   \
                if (!(assertion)) {                                            \
                        printf("[ASSERT] '" #assertion "'\n");                 \
                }                                                              \
        } while (0)

#else // DEBUG

#define assert(...)

#endif // DEBUG

#endif
