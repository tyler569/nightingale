
#ifndef NIGHTINGALE_LIB_DEBUG_STUFF
#define NIGHTINGALE_LIB_DEBUG_STUFF

#define HEAPDBG_SUMMARY 2
#define HEAPDBG_DETAIL  1

// debug the kernel heap
int heapdbg(int type);

// debug the local heap
void print_pool();
void summarize_pool();

#endif

