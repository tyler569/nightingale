
#include <basic.h>
extern "C" {
#include <stdlib.h>
}
#include <types.hh>

void *operator new(nx::size_t size) {
    return malloc(size);
}

void *operator new[](nx::size_t size) {
    return malloc(size);
}

void operator delete(void *ptr) {
    free(ptr);
}

void operator delete(void *ptr, nx::size_t size) {
    (void)size;
    free(ptr);
}

void operator delete[](void *ptr) {
    free(ptr);
}

void operator delete[](void *ptr, nx::size_t size) {
    (void)size;
    free(ptr);
}

