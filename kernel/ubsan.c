#include <basic.h>
#include <ng/debug.h>
#include <ng/panic.h>
#include <stdio.h>

struct source_location {
    const char *file_name;
    uint32_t line;
    uint32_t column;
};

struct typedesc {
    uint16_t type_kind;
    uint16_t type_info;
    char type_name[1];
};

struct overflow_info {
    struct source_location loc;
    struct typedesc *type;
};

struct invalid_value_info {
    struct source_location loc;
    struct typedesc *type;
};

struct shift_oob_info {
    struct source_location loc;
    struct typedesc *lhs_type;
    struct typedesc *rhs_type;
};

struct type_mismatch_info {
    struct source_location loc;
    struct typedesc *type;
    uintptr_t alignment;
    uint8_t type_check_kind;
};

struct type_mismatch_info_v1 {
    struct source_location loc;
    struct typedesc *type;
    uint8_t log_alignment;
    uint8_t type_check_kind;
};

struct out_of_bounds_info {
    struct source_location loc;
    struct typedesc *array_type;
    struct typedesc *index_type;
};

struct pointer_overflow_info {
    struct source_location loc;
};

struct nonnull_arg_info {
    struct source_location loc;
    struct source_location attr_loc;
    int arg_index;
};

static void print_sloc(struct source_location *sloc)
{
    printf("  at: %s:%u:%u\n", sloc->file_name, sloc->line, sloc->column);
}

__USED
void __ubsan_handle_negate_overflow(
    struct overflow_info *info, unsigned long value)
{
    printf("\nubsan: negate overflow detected\n");
    printf("  value: %lu, type: %s\n", value, info->type->type_name);
    print_sloc(&info->loc);
    panic_bt("ubsan");
}

__USED
void __ubsan_handle_add_overflow(struct overflow_info *info,
    unsigned long value_lhs, unsigned long value_rhs)
{
    printf("ubsan: add overflow detected\n");
    printf("  lhs: %lu, rhs: %lu, type: %s\n", value_lhs, value_rhs,
        info->type->type_name);
    print_sloc(&info->loc);
    panic_bt("ubsan");
}

__USED
void __ubsan_handle_sub_overflow(struct overflow_info *info,
    unsigned long value_lhs, unsigned long value_rhs)
{
    printf("ubsan: sub overflow detected\n");
    printf("  lhs: %lu, rhs: %lu, type: %s\n", value_lhs, value_rhs,
        info->type->type_name);
    print_sloc(&info->loc);
    panic_bt("ubsan");
}

__USED
void __ubsan_handle_mul_overflow(struct overflow_info *info,
    unsigned long value_lhs, unsigned long value_rhs)
{
    printf("\nubsan: mul overflow detected\n");
    printf("  lhs: %lu, rhs: %lu, type: %s\n", value_lhs, value_rhs,
        info->type->type_name);
    print_sloc(&info->loc);
    panic_bt("ubsan");
}

__USED
void __ubsan_handle_divrem_overflow(struct overflow_info *info,
    unsigned long value_lhs, unsigned long value_rhs)
{
    printf("\nubsan: divrem overflow detected\n");
    printf("  lhs: %lu, rhs: %lu, type: %s\n", value_lhs, value_rhs,
        info->type->type_name);
    print_sloc(&info->loc);
    panic_bt("ubsan");
}

__USED
void __ubsan_handle_load_invalid_value(
    struct invalid_value_info *info, unsigned long value)
{
    printf("\nubsan: load invalid detected\n");
    printf("  value: %lu, type: %s\n", value, info->type->type_name);
    print_sloc(&info->loc);
    panic_bt("ubsan");
}

__USED
void __ubsan_handle_shift_out_of_bounds(struct shift_oob_info *info,
    unsigned long value_lhs, unsigned long value_rhs)
{
    printf("ubsan: shift out of bounds detected\n");
    printf("  lhs: (%s) %lu\n", info->lhs_type->type_name, value_lhs);
    printf("  rhs: (%s) %lu\n", info->rhs_type->type_name, value_rhs);
    print_sloc(&info->loc);
    panic_bt("ubsan");
}

__USED
void __ubsan_handle_pointer_overflow(struct pointer_overflow_info *info)
{
    return; // DISABLED

    printf("ubsan: pointer overflow detected\n");
    print_sloc(&info->loc);
    panic_bt("ubsan");
}

static const char *type_check_kinds[] = {
    "load of",
    "store to",
    "reference binding to",
    "member access within",
    "member call on",
    "constructor call on",
    "downcast of",
    "downcast of",
    "upcast of",
    "cast to virtual base of",
};

__USED
void __ubsan_handle_type_mismatch_v1(
    struct type_mismatch_info_v1 *info, unsigned long pointer)
{
    return; // DISABLED

    printf("ubsan: type mismatch\n");
    printf("  %s %p (type %s)\n", type_check_kinds[info->type_check_kind],
        (void *)pointer, info->type->type_name);
    print_sloc(&info->loc);
    panic_bt("ubsan");
}

__USED
void __ubsan_handle_builtin_unreachable()
{
    printf("ubsan: __builtin_unreachable reached\n");
    printf("INFO TODO\n");
    panic_bt("ubsan");
}

__USED
void __ubsan_handle_out_of_bounds()
{
    printf("ubsan: out of bounds access detected\n");
    printf("INFO TODO\n");
    panic_bt("ubsan");
}

__USED
void __ubsan_handle_nonnull_arg(struct nonnull_arg_info *info)
{
    printf("ubsan: NULL passed to function argument marked nonnull\n");
    printf("  argument %i\n", info->arg_index);
    print_sloc(&info->loc);
}
