
#include <basic.h>
#include <print.h>
#include <panic.h>
#include <debug.h>

void __ubsan_handle_negate_overflow() {
    backtrace_from_here(20);
    panic("ubsan: negate overflow detected\n");
}
void __ubsan_handle_add_overflow() {
    backtrace_from_here(20);
    panic("ubsan: add overflow detected\n");
}
void __ubsan_handle_sub_overflow() {
    backtrace_from_here(20);
    panic("ubsan: sub overflow detected\n");
}
void __ubsan_handle_mul_overflow() {
    backtrace_from_here(20);
    panic("ubsan: mul overflow detected\n");
}
void __ubsan_handle_divrem_overflow() {
    backtrace_from_here(20);
    panic("ubsan: divrem overflow detected\n");
}
void __ubsan_handle_load_invalid_value() {
    backtrace_from_here(20);
    panic("ubsan: load invalid detected\n");
}
void __ubsan_handle_shift_out_of_bounds() {
    backtrace_from_here(20);
    panic("ubsan: shift out of bounds detected\n");
}

