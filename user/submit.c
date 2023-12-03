#include "ng/submission_q.h"
#include "ng/syscall_consts.h"
#include <stdio.h>
#include <syscall.h>

struct submission q[] = {
    { NG_WRITE, { 1, (uintptr_t) "Hello World\n", 12 } },
    { NG_WRITE, { 1, (uintptr_t) "Hello ABC\n", 10 } },
    { NG_WRITE, { 1, (uintptr_t) "Hello queue\n", 12 } },
};

int __ng_submit(struct submission *, size_t);

int main() { __ng_submit(q, 3); }
