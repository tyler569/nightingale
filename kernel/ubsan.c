
#include <basic.h>
#include <print.h>
#include <panic.h>

void __ubsan_handle_negate_overflow() {
    panic("ubsan: negate overflow detected\n");
}
void __ubsan_handle_add_overflow() {
    panic("ubsan: add overflow detected\n");
}
void __ubsan_handle_sub_overflow() {
    panic("ubsan: sub overflow detected\n");
}
void __ubsan_handle_mul_overflow() {
    panic("ubsan: mul overflow detected\n");
}
void __ubsan_handle_divrem_overflow() {
    panic("ubsan: divrem overflow detected\n");
}
void __ubsan_handle_load_invalid_value() {
    panic("ubsan: load invalid detected\n");
}
void __ubsan_handle_shift_out_of_bounds() {
    panic("ubsan: shift out of bounds detected\n");
}

