
#include <ng/basic.h>
#include <ng/debug.h>
#include <ng/panic.h>
#include <ng/print.h>
#include <ng/ubsan.h>

void print_sloc(struct source_location *sloc) {
        printf("  at: %s:%li:%li\n", sloc->file_name, sloc->line, sloc->column);
}

USED void __ubsan_handle_negate_overflow(struct ubsan_overflow_info *info,
                                         unsigned long value) {
        printf("\nubsan: negate overflow detected\n");
        printf("  value: %lu, type: %s\n", value, &(info->type->type_name));
        print_sloc(&(info->loc));
        backtrace_from_here(20);
        panic("");
}

USED void __ubsan_handle_add_overflow(struct ubsan_overflow_info *info,
                                      unsigned long value_lhs,
                                      unsigned long value_rhs) {
        printf("ubsan: add overflow detected\n");
        printf("  lhs: %lu, rhs: %lu, type: %s\n", value_lhs, value_rhs,
               &(info->type->type_name));
        print_sloc(&(info->loc));
        backtrace_from_here(20);
        panic("");
}

USED void __ubsan_handle_sub_overflow(struct ubsan_overflow_info *info,
                                      unsigned long value_lhs,
                                      unsigned long value_rhs) {
        printf("ubsan: sub overflow detected\n");
        printf("  lhs: %lu, rhs: %lu, type: %s\n", value_lhs, value_rhs,
               &(info->type->type_name));
        print_sloc(&(info->loc));
        backtrace_from_here(20);
        panic("");
}

USED void __ubsan_handle_mul_overflow(struct ubsan_overflow_info *info,
                                      unsigned long value_lhs,
                                      unsigned long value_rhs) {
        printf("\nubsan: mul overflow detected\n");
        printf("  lhs: %lu, rhs: %lu, type: %s\n", value_lhs, value_rhs,
               &(info->type->type_name));
        print_sloc(&(info->loc));
        backtrace_from_here(20);
        panic("");
}

USED void __ubsan_handle_divrem_overflow(struct ubsan_overflow_info *info,
                                         unsigned long value_lhs,
                                         unsigned long value_rhs) {
        printf("\nubsan: divrem overflow detected\n");
        printf("  lhs: %lu, rhs: %lu, type: %s\n", value_lhs, value_rhs,
               &(info->type->type_name));
        print_sloc(&(info->loc));
        backtrace_from_here(20);
        panic("");
}

USED void
__ubsan_handle_load_invalid_value(struct ubsan_invalid_value_info *info,
                                  unsigned long value) {
        printf("\nubsan: load invalid detected\n");
        printf("  value: %lu, type: %s\n", value, &(info->type->type_name));
        print_sloc(&(info->loc));
        backtrace_from_here(20);
        panic("");
}

USED void __ubsan_handle_shift_out_of_bounds(struct ubsan_shift_oob_info *info,
                                             unsigned long value_lhs,
                                             unsigned long value_rhs) {
        printf("ubsan: shift out of bounds detected\n");
        printf("  lhs: (%s) %lu\n", &(info->lhs_type->type_name[0]), value_lhs);
        printf("  rhs: (%s) %lu\n", &(info->rhs_type->type_name[0]), value_rhs);
        print_sloc(&(info->loc));
        backtrace_from_here(20);
        panic("");
}

USED void __ubsan_handle_pointer_overflow() {}
USED void __ubsan_handle_type_mismatch_v1() {}
USED void __ubsan_handle_builtin_unreachable() {}
USED void __ubsan_handle_out_of_bounds() {}
