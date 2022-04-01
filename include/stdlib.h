#pragma once
#ifndef _STDLIB_H_
#define _STDLIB_H_

#include <sys/cdefs.h>
#include <list.h>

#if __kernel__
#include <ng/sync.h>
#endif

#define EXIT_SUCCESS (0)
#define EXIT_FAILURE (1)

#define RAND_MAX 0xFFFFFFFF

#define ATEXIT_MAX 32

#define HEAP_MINIMUM_BLOCK 16
#define HEAP_MINIMUM_ALIGN 16

void free(void *alloc);
void *malloc(size_t len) __MALLOC(1);
void *realloc(void *alloc, size_t len) __MALLOC(2);
void *calloc(size_t count, size_t len) __MALLOC(1, 2);
void *zmalloc(size_t len) __MALLOC(1);
void *zrealloc(void *, size_t) __MALLOC(2);

void __nc_malloc_init(void);
long int strtol(const char *nptr, char **endptr, int base);
long long int strtoll(const char *nptr, char **endptr, int base);
unsigned long strtoul(const char *nptr, char **endptr, int base);
unsigned long long strtoull(const char *ntr, char **endptr, int base);

void qsort(void *base, size_t nmemb, size_t size,
    int (*compar)(const void *, const void *));

#if __kernel__
#define EARLY_MALLOC_POOL_LEN 128 * 1024
extern char early_malloc_pool[EARLY_MALLOC_POOL_LEN];
#endif

struct mheap;
extern struct mheap *__global_heap_ptr;

void heap_init(struct mheap *, void *base, size_t len);
void *heap_malloc(struct mheap *, size_t len);
void heap_free(struct mheap *, void *alloc);
void *heap_realloc(struct mheap *, void *alloc, size_t len);
void *heap_calloc(struct mheap *, size_t count, size_t len);

/*
   void *__location_malloc(size_t len, const char *location);
   void *__location_zmalloc(size_t len, const char *location);
   void *__location_realloc(void *allocation, size_t len, const char *location);
   void __location_free(void *allocation, const char *location);

 #if _NC_LOCATION_MALLOC
 #define malloc(length) __location_malloc(length, __FILE__ ":" QUOTE(__LINE__));
 #define zmalloc(length) __location_zmalloc(length, __FILE__ ":"
 QUOTE(__LINE__)); #define realloc(allocation, length)
 __location_realloc(allocation, length, __FILE__ ":" QUOTE(__LINE__)); #define
 free(allocation) __location_free(allocation, __FILE__ ":" QUOTE(__LINE__));
 #endif
 */

#ifndef __kernel__

int abs(int x);
long labs(long x);
long long llabs(long long x);

struct div_t {
    int quot;
    int rem;
};
struct ldiv_t {
    long quot;
    long rem;
};
struct lldiv_t {
    long long quot;
    long long rem;
};

typedef struct div_t div_t;
typedef struct ldiv_t ldiv_t;
typedef struct lldiv_t lldiv_t;

div_t div(int x, int y);
ldiv_t ldiv(long x, long y);
lldiv_t lldiv(long long x, long long y);

char *getenv(const char *name);
void abort(void);

long int random(void);
void srandom(unsigned seed);
int rand(void);
void srand(unsigned seed);

double strtod(const char *str, char **end);
float strtof(const char *str, char **end);
long double strtold(const char *str, char **end);
int atoi(const char *nptr);
long atol(const char *nptr);
long long atoll(const char *nptr);

_Noreturn void _exit(int status);
_Noreturn void exit(int status);
_Noreturn void exit_group(int status);
int atexit(void (*fn)(void));

// TODO

int system(const char *command);
int mkstemp(char *name);

#endif // !__kernel__

#endif // _STDLIB_H_
