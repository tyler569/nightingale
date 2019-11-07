
#include <basic.h>
#include <ng/print.h>
#include <ng/timer.h>

// TODO : arch specific
#include <ng/x86/pit.h>

int interrupt_in_ns(long nanoseconds) {
        return pit_create_oneshot(nanoseconds);
}

long kernel_timer = 0;

void timer_callback() {
        kernel_timer += 1;
}

