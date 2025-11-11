/**
 * @file
 * @brief Handlers for -fsanitize=undefined flag
 *
 * @date   13 Apr 2023
 * @author Denis Deryugin
 */

#include <framework/mod/options.h>
#include <hal/cpu_idle.h>
#include <kernel/panic.h>
#include <kernel/printk.h>
#include <util/location.h>

#define STOP_ON_HANDLE OPTION_GET(BOOLEAN, stop_on_handle)

static void print_ubsan_data(void *data) {
	/* Assume every handler-specific data member starts with location */
	struct location *l = data;
	printk("%s:%d\n", l->file, l->line);

#if STOP_ON_HANDLE
	panic("UbSan handle\n");
#endif
}

void __ubsan_handle_add_overflow(void *data, void *lhs, void *rhs) {
	printk("UbSan add overflow ");
	print_ubsan_data(data);
}

void __ubsan_handle_sub_overflow(void *data, void *lhs, void *rhs) {
	printk("UbSan sub overflow ");
	print_ubsan_data(data);
}

void __ubsan_handle_mul_overflow(void *data, void *lhs, void *rhs) {
	printk("UbSan mul overflow ");
	print_ubsan_data(data);
}

void __ubsan_handle_negate_overflow(void *data, void *val) {
	printk("UbSan negate overflow ");
	print_ubsan_data(data);
}

void __ubsan_handle_divrem_overflow(void *data, void *lhs, void *rhs) {
	printk("UbSan divrem overflow ");
	print_ubsan_data(data);
}

void __ubsan_handle_pointer_overflow(void *data, void *base, void *result) {
	printk("UbSan pointer overflow ");
	print_ubsan_data(data);
}

void __ubsan_handle_alignment_assumption(void *data, void *ptr, void *align,
    void *offset) {
	printk("UbSan alignment assumption ");
	print_ubsan_data(data);
}

void __ubsan_handle_float_cast_overflow(void *data, void *from) {
	printk("UbSan float cast overflow ");
	print_ubsan_data(data);
}

void __ubsan_handle_function_type_mismatch(void *data, void *func) {
	printk("UbSan function type mismatch ");
	print_ubsan_data(data);
}

void __ubsan_handle_type_mismatch(void *data, void *type) {
	printk("UbSan type mismatch ");
	print_ubsan_data(data);
}

void __ubsan_handle_type_mismatch_v1(void *data, void *type) {
	printk("UbSan type mismatch ");
	print_ubsan_data(data);
}

#if defined __GNUC__ && __GNUC__ < 6
void __ubsan_handle_nonnull_arg(void *data, int num)
#else
void __ubsan_handle_nonnull_arg(void *data)
#endif
{
	printk("UbSan nonnull arg ");
	print_ubsan_data(data);
}

void __ubsan_handle_shift_out_of_bounds(void *data, void *lhs, void *rhs) {
	printk("UbSan shift out of bounds (value = %p; shift = %p) ", lhs, rhs);
	print_ubsan_data(data);
}

void __ubsan_handle_out_of_bounds(void *data, void *base) {
	printk("UbSan out of bounds ");
	print_ubsan_data(data);
}

void __ubsan_handle_invalid_builtin(void *data) {
	printk("UbSan invalid builtin ");
	print_ubsan_data(data);
}

void __ubsan_handle_builtin_unreachable(void *data) {
	printk("UbSan builtin unreachable");
	print_ubsan_data(data);
	while (1) {
		arch_cpu_idle();
	}
}

void __ubsan_handle_load_invalid_value(void *data, void *base) {
	printk("UbSan load invalid value ");
	print_ubsan_data(data);
}
