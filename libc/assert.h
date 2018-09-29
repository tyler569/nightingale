
#ifndef _ASSERT_H_
#define _ASSERT_H_

#ifndef NDEBUG

# define assert(assertion, message) \
    do { \
        if (!(assertion)) { \
            printf("[ASSERT] '" #assertion "' - " message "\n"); \
        } \
    while (0)

#else 

# define assert(...)

#endif

#endif

