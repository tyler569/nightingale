
#include <ng/basic.h>
#include <ng/print.h>
#include <ng/timer.h>

// TODO : arch specific
#include <arch/x86/pit.h>

int interrupt_in_ns(long nanoseconds) {
    return pit_create_oneshot(nanoseconds);
}

