
#include <basic.h>
#include <stdio.h>
#include <arch/x86/pit.h>
#include "timer.h"

int interrupt_in_ns(long nanoseconds) {
    return pit_create_oneshot(nanoseconds);
}

